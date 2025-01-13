#include <iostream>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>  // Memory
#include <asio/ts/internet.hpp>  // Network

int main() {
	asio::error_code ec;
	asio::io_context context;  // for platform independence

	// tcp endpoint
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("111.11.11.111", ec), 80);
	return 0;
}