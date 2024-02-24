#include <iostream>
#include <boost/asio.hpp>
#include <future>
#include <thread>
#include <unistd.h>

using boost::asio::ip::udp;

void async_recive(udp::endpoint receiver_endpoint, udp::socket* socket) {
	std::array<char, 128> recv_buf;
    socket->async_receive_from(boost::asio::buffer(recv_buf), receiver_endpoint, [](const boost::system::error_code& error, std::size_t bytes_received) {
		if (!error) {
			std::cout << "Received " << bytes_received << " bytes asynchronously.\n" << std::endl;
		} else {
			std::cerr << "Receive failed: " << error.message() << std::endl;
		}
	});
}

void sendMessage(udp::endpoint receiver_endpoint, udp::socket* socket) {
	while (true) {
		std::string message;
		std::cout << "Message to send to server: ";
		std::cin >> message;
		// Async send
		socket->async_send_to(boost::asio::buffer(message), receiver_endpoint, [](const boost::system::error_code& error, std::size_t bytes_transferred) {
			if (!error) {
				std::cout << "Sent " << bytes_transferred << " bytes asynchronously." << std::endl;
			} else {
				std::cerr << "Send failed: " << error.message() << std::endl;
			}
		});
	}
}

int main() {
    boost::asio::io_context io_context;
    udp::resolver resolver(io_context);
    udp::endpoint receiver_endpoint = *resolver.resolve(udp::v4(), "127.0.0.1", "48").begin();
    udp::socket socket(io_context);
    socket.open(udp::v4());
	// Async send
	std::thread asyncSend(sendMessage, receiver_endpoint, &socket);
	asyncSend.detach();
	// Async receive
	std::thread asyncReadThread(async_recive, receiver_endpoint, &socket);
	asyncReadThread.detach();
	io_context.run();
	while (true) {Sleep(10);}
    return 0;
}
