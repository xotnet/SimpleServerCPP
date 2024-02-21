#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class UDPServer {
	public:
		UDPServer(boost::asio::io_context& io_context) : socket_(io_context, udp::endpoint(udp::v4(), 48)) {
			doReceive();
		}

	private:
		void doReceive() {
			socket_.async_receive_from(
				boost::asio::buffer(recv_buffer_), remote_endpoint_,
				[this](const boost::system::error_code& error, std::size_t bytes_recvd) {
					if (!error) {
						std::cout << "Received " << bytes_recvd << " bytes." << std::endl;
						doSend(bytes_recvd);
					} else {
						std::cerr << "Error receiving data: " << error.message() << std::endl;
						doReceive();
					}
				});
		}

		void doSend(std::size_t length) {
			socket_.async_send_to(
				boost::asio::buffer(recv_buffer_, length), remote_endpoint_,
				[this](const boost::system::error_code& error, std::size_t bytes_sent) {
					if (!error) {
						std::cout << "Sent " << bytes_sent << " bytes." << std::endl;
						doReceive();  // After data (message) gotten continue recive data
					} else {
						std::cerr << "Error sending data: " << error.message() << std::endl;
						doReceive();
					}
				});
		}

		udp::socket socket_;
		udp::endpoint remote_endpoint_;
		std::array<char, 1024> recv_buffer_;
};
int main() {
    try {
        boost::asio::io_context io_context;
        UDPServer server(io_context);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}