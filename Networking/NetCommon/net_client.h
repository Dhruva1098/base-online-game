#pragma once

#include "net_common.h"
#include "net_thread_safe_queue.h"
#include "net_message.h"
#include "net_connection.h"

template <typename T>
class client_interface {
	client_interface() : m_socket(m_context) {}
	virtual ~client_interface() {
		Disconnect();
	}
public:
	// connect to server with ip and port
	bool Connect(const std::string& host, const uint16_t port) {
		try {
			//create connection
			m_connection = std::make_unique<connection<T>>();  //wip

			//resolve ip into physical address
			asio::ip::tcp::resolver resolver(m_context);
			m_endpoints = resolver.resolve(host, std::to_string(port));

			// connect to server
			m_connection->ConnectToServer(m_endpoints);

			// start context thread
			thrContext = std::thread([this]() { m_context.run(); });
		}
		catch (std::exception& e) {
			std::cerr << "Client Exception: " << e.what() << "\n";
			return false;
		}
		return false;
	}
	void Disconnect() {
		if (IsConnected()) {
			m_connection->Disconnect();
		}
		// make sure we are done with asio context and thread
		m_context.stop();
		if (thrContext.joinable()) thrContext.join();

		//destroy connection obj
		m_connection.release();
	}	
	bool IsConnected() {
		if (m_connection) return m_connection->IsConnected();
		else return false;
	}

	//retrieve queue messages from the server
	tsqueue<owned_message<T>>& Incoming() {
		return m_qMessagesIn;
	}

protected:
	// asio context handles data transfer
	asio::io_context m_context;
	// but needs a thread to execute work commands
	std::thread thrContext;
	// socket connected to server
	asio::ip::tcp::socket m_socket;
	// also single instance of connection obk which handles data transfer
	std::unique_ptr<connection<T>> m_connection;

private:
	tsqueue<owned_message<T>> m_qMessagesIn;
};