#include <iostream>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>  // Memory
#include <asio/ts/internet.hpp>  // Network

int main() {
	std::cout << "hello world";
	return 0;
}