#include <iostream>
#include <boost/asio.hpp>
#include <ctime>
#include <string>

/*
    Second Iteration: Asynchronous TCP server 
    Test using "nc localhost 8080" from another terminal

    Offical Doc: https://www.boost.org/doc/libs/latest/doc/html/boost_asio/tutorial/tutdaytime3.html
*/
using namespace std;
using boost::asio::ip::tcp;


// enable_shared_from_this is a class that u can inherit so that an object can safely create additional shared_ptr
// to itself, but only if the object is already owned by a shared_ptr
class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
    public:
        typedef std::shared_ptr<tcp_connection> pointer;

        // static so that it can be called without an instance of tcp_connection, as its purpose
        // is to create an instance of tcp_connection and pass it back as a pointer
        static pointer create(boost::asio::io_context& io_context)
        {
            return pointer(new tcp_connection(io_context)); // shared_ptr created around a raw pointer, new returns a raw pointer
        }

        tcp::socket& socket()
        {
            return socket_;
        }

        void start()
        {
            message_ = "hello, testing from async server";

            // // old way of writing it
            // boost::asio::async_write
            // (
            //     socket_, boost::asio::buffer(message_),
            //     std::bind
            //     (
            //         &tcp_connection::handle_write, // pointer to a member function 
            //         shared_from_this(), // this appears for every non-static member function call
            //         boost::asio::placeholders::error,
            //         boost::asio::placeholders::bytes_transferred
            //     )
            // );

            // new way of writing it with lambda
            auto self = shared_from_this();
            boost::asio::async_write
            (
                socket_, boost::asio::buffer(message_),
                // lambda captures self which is a smart pointer to itself, then calls its member function handle_write 
                [self] (const boost::system::error_code& ec, size_t bytes)
                {
                    self->handle_write(ec, bytes);
                }
            );
        }

    private:
        tcp::socket socket_;
        std::string message_;

        tcp_connection(boost::asio::io_context& io_context) : socket_(io_context)
        {
            ;
        }

        void handle_write(const boost::system::error_code&, size_t)
        {
            ;
        }
};

class tcp_server
{
    public: 
        tcp_server(boost::asio::io_context& io_context) : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 8080))
        {
            start_accept();
        }
    private:
        // must be a reference as io_context is the program's link to OS's I/O services, has to outlive all async operations  
        boost::asio::io_context& io_context_; 
        tcp::acceptor acceptor_;
        
        // notice the infinite accept loop here with start_accept() and handle_accept()
        void start_accept()
        {
            tcp_connection::pointer new_connection = tcp_connection::create(io_context_);


            acceptor_.async_accept
            (
                new_connection->socket(),
                [this, new_connection] (const boost::system::error_code& ec)
                {
                    handle_accept(new_connection, ec);
                }
            );
            

            // note: this is the official code but it breaks due to boost::asio::placeholders::error being implemented as a function
            // instead of actual object async_accept was supposed to call the callable object with handler(ec) where ec is the error code, an actual object
            // acceptor_.async_accept
            // (
            //     new_connection->socket(),
            //     std::bind
            //     (
            //         &tcp_server::handle_accept,
            //         this,
            //         new_connection,
            //         boost::asio::placeholders::error
            //     )
            // );
        }
        void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
        {
            if(!error) new_connection->start();
            start_accept();
        }
};



int main()
{

    try
    {
        // I/O execution context represents the program's link to the operating system's I/O services.
        boost::asio::io_context io_context; 
        tcp_server server(io_context);

        // .run() starts the io_context's event processing loop that looks at async operations
        // without doing .run() even if async operations are scheduled (via async_accept), nothing will process/dispatch
        // however .run() will immediately return when there is no work. here it doesnt immediately return 
        // as constructor of tcp_server does start_accept() that runs async_accept which 
        // registers an asynchronous operation with the io_context/OS before we reach io_context.run()
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    // try
    // {
    //     // I/O execution context represents the program's link to the operating system's I/O services.
    //     boost::asio::io_context io_context; 
    //     /*
    //         tcp::aceptor represents a listening socket.
    //         Here we are listening to every ipv4 address on this machine 
    //         -> servers usually do this 
    //         tcp::endpoint(tcp::v4(), 8080) ==> 0.0.0.0:8080
    //     */
    //     tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
    //     while(1)
    //     {
    //         // empty socket that will be associated to the client when there is an incoming connection
    //         // it is constructed with io_context to link the socket with this I/O execution context
    //         tcp::socket socket(io_context); 
    //         acceptor.accept(socket); // blocks while it waits for incoming client

    //         string message = "testing2";
    //         boost::system::error_code error_code; // this is ignored

    //         // boost::asio::write designed to work with raw memory regions, not C++ containers
    //         // hence u cant just pass a std::string straight in, we need boost::asio::buffer() to aid us
    //         boost::asio::write(socket, boost::asio::buffer(message), error_code);
    //     }
    // } 
    // catch (std::exception& e)
    // {
    //     std::cerr << e.what() << std::endl;
    // }
    
    return 0;
}