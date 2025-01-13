#include <iostream>
#include <vector>
#include <string>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>  // Memory
#include <asio/ts/internet.hpp>  // Network

std::vector<char> async_buff(20 * 1024);

void GrabData(asio::ip::tcp::socket& socket) {
	socket.async_read_some(asio::buffer(async_buff.data(), async_buff.size()),
		[&](std::error_code ec, std::size_t length) {
			if (!ec) {
				std::cout << "\nRead" << length << " Bytes\n";
				for (int n = 0; n < length; n++) {
					std::cout << async_buff[n];
				}
				GrabData(socket);
			}
		});
}

int main() {
	asio::error_code ec;
	asio::io_context context;  // for platform independence
	asio::io_context::work idleWork(context);
	std::thread	thrContext = std::thread([&]() {context.run();});


	// tcp endpoint
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

	// socket
	asio::ip::tcp::socket socket(context);

	// connect
	socket.connect(endpoint, ec);
	if (!ec) std::cout << "Connected\n";
	else std::cout << "Failed\n" << ec.message() << std::endl;

	if (socket.is_open()) {
		GrabData(socket);

		std::string request = "GET /index.html HTTP/1.1\r\n"
			"Host: david-barr.co.uk\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(request.data(), request.size()), ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);
	}


	return 0;
}