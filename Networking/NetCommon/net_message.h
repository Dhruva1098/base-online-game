// Defination of our message object
#pragma once
#include "net_common.h"

template <typename T>
struct message_header {
	T id{};
	uint32_t size = 0;
};

template <typename T>
struct message {
	message_header<T> header{};
	std::vector<uint8_t> body;

	// return size of message packet
	size_t size() const {
		return sizeof(message_header<T>) + body.size();
	}

	// Override for std::cout compatibility
	friend std::ostream& operator << (std::ostream& os, const message<T>& msg) {
		os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
		return os;
	}

	// Push using <<
	template<typename DataType>  // Because we want to be able to puch any data type
	friend message<T>& operator << (message<T>& msg, const DataType& data) {
		// check if type provided is copyable
		static_assert(std::is_standard_layout<DataType>::value, "Data is too complex for a vector");

		// cache current suze of vector, this is where we insert data
		size_t i = msg.body.size();

		// Resize vector
		msg.body.resize(msg.body.size() + sizeof(DataType));

		// copy data into newly allocated space
		std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

		// update size 
		msg.header.size = msg.size();

		return msg;
	}

	// Pop using >> 
	template<typename DataType>
	friend message<T>& operator >> (message<T>& msg, DataType& data) {
		static_assert(std::is_standard_layout<DataType>::value, "data is too complex");

		// cache to where our data starts
		size_t i = msg.body.size() - sizeof(DataType);

		// copy
		std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

		// shrink vector
		msg.body.resize(i);

		return msg;

	}
};

template <typename T>
class connection;

template <typename T>
struct owned_message {
	std::shared_ptr<connection<T>> remote = nullptr;
	message<T> msg;
	
	// cout compability
	friend std::ostream& operator << (std::ostream& os, const owned_message<T>& msg) {
		os << msg.msg;
		return os;
	}

};

