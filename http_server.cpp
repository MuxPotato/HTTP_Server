#include <iostream>
#include <boost/asio.hpp>

using namespace std;
int main()
{
    boost::asio::io_context io;
    std::cout << "Boost.Asio OK\n";
    return 0;
}