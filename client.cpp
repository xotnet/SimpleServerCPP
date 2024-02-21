#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

int main() {
    boost::asio::io_context io_context;

    udp::resolver resolver(io_context);
    udp::endpoint receiver_endpoint = *resolver.resolve(udp::v4(), "127.0.0.1", "48").begin();

    udp::socket socket(io_context);

    socket.open(udp::v4());

    // Async send
    std::string send_buf = "Hello, server!";
    socket.async_send_to(boost::asio::buffer(send_buf), receiver_endpoint,
        [](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::cout << "Sent " << bytes_transferred << " bytes asynchronously." << std::endl;
            } else {
                std::cerr << "Send failed: " << error.message() << std::endl;
            }
        });

    // Async receive
    std::array<char, 128> recv_buf;
    socket.async_receive_from(
        boost::asio::buffer(recv_buf), receiver_endpoint,
        [](const boost::system::error_code& error, std::size_t bytes_received) {
            if (!error) {
                std::cout << "Received " << bytes_received << " bytes asynchronously." << std::endl;
            } else {
                std::cerr << "Receive failed: " << error.message() << std::endl;
            }
        });

    io_context.run();
    return 0;
}