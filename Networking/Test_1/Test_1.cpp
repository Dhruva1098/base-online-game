#include <iostream>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>  // Memory
#include <asio/ts/internet.hpp>  // Network

int main() {
	asio::error_code ec;
	asio::io_context context;  // for platform independence

	// tcp endpoint
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 80);

	// socket
	asio::ip::tcp::socket socket(context);

	// connect
	socket.connect(endpoint, ec);
	if (!ec) std::cout << "Connected\n";
	else std::cout << "Failed\n" << ec.message() << std::endl;


	return 0;
}