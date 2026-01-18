#include <iostream>
#include <boost/asio.hpp>
#include <ctime>
#include <string>

/*
    First Iteration: Synchronous TCP server first
*/

using namespace std;
using boost::asio::ip::tcp;
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