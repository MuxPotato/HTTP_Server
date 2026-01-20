#include <iostream>
#include <boost/asio.hpp>
#include <ctime>
#include <string>

/*
    Second Iteration: Asynchronous TCP server 
*/
using namespace std;
using boost::asio::ip::tcp;


class tcp_server
{
    public: 
        tcp_server(boost::asio::io_context& io_context) : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 13))
        {
            start_accept();
        }
    private:
        // must be a reference as io_context is the program's link to OS's I/O services, has to outlive all async operations  
        boost::asio::io_context& io_context_; 
        tcp::acceptor acceptor_;

        void start_accept()
        {
            ;
        }
};

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




}






int main()
{
    try
    {
        // I/O execution context represents the program's link to the operating system's I/O services.
        boost::asio::io_context io_context; 
        /*
            tcp::aceptor represents a listening socket.
            Here we are listening to every ipv4 address on this machine 
            -> servers usually do this 
            tcp::endpoint(tcp::v4(), 8080) ==> 0.0.0.0:8080
        */
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        while(1)
        {
            // empty socket that will be associated to the client when there is an incoming connection
            // it is constructed with io_context to link the socket with this I/O execution context
            tcp::socket socket(io_context); 
            acceptor.accept(socket); // blocks while it waits for incoming client

            string message = "testing2";
            boost::system::error_code error_code; // this is ignored

            // boost::asio::write designed to work with raw memory regions, not C++ containers
            // hence u cant just pass a std::string straight in, we need boost::asio::buffer() to aid us
            boost::asio::write(socket, boost::asio::buffer(message), error_code);
        }
    } 
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}