#pragma once

#include "net_common.h"
#include "net_thread_safe_queue.h"
#include "net_message.h"

template<typename T>
class connection : public std::enable_shared_from_this<connection<T>> {
public:
	enum class	owner {
		server,
		client
	};

	connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
		: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
	{
		m_nOwnerType = parent;
	}

	virtual ~connection() {}

	uint32_t GetID() const {
		return id;
	}

public:

	void ConnectToClient(uint32_t uid = 0) {
		if (m_nOwnerType == owner::server) {
			if (m_socket.is_open()) {
				id = uid;
				ReadHeader();
			}
		}
	}
	bool ConnectToServer();  // can only be called by client (duh)
	bool Disconnect();
	
	bool IsConnected() const {
		return m_socket.is_open();
	}

public:
	bool Send(const message<T>& msg) {
		asio::post(m_asioContext,
			[this, msg]() {
				// I cannot just do WriteHeader() because message does not exists in my message queue yet
				bool bWritingMessage = !m_qMessagesOut.empty(); // make sure we are not writing message already
				m_qMessagesOut.push_back(msg);
				if(bWritingMessage){
					WriteHeader();
				}
			});
	}

privete:
	// ASYNC: read message headers
	void ReadHeader() {
		asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
			[this](std::error_code ec, std::size_t length) {
				if (!ec) {
					if (m_msgTemporaryIn.header.size > 8) {  // there is a body, 8 byte is header so we d0 >8
						m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size - 8);
						ReadBody();
					}
					else {
						AddToIncomingMessageQueue();
					}

				}
				else {
					std::cout << "[" << id << "] Read Header Fail\n";
					m_socket.close();
				});
			}
	}

	// read body
	void ReadBody() {
		asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
			[this](std::error_code ec, std::size_t length) {
				if (!ec) {
					AddToIncomingMessageQueue();
				}
				else {
					std::cout << "[" << id << "] Read Body Failed\n";
					m_socket.close()
				}
			});
	}
	
	// write Header
	void WriteHeader() {
		asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
			[this](std::error_code ec, std::size_t length) {
				if (!ec) {
					if (m_qMessagesOut.front().body.size() > 0) {
						WriteBody();
					}
					else {
						m_qMessagesOut.pop_front();  // done with message header
						if (!m_qMessagesOut.empty()) {
							WriteHeader();
						}
					}
				} 
				else {
					std::cout << "[" << id << "] Write HEader fail.\n";
					m_socket.close();
				}
			}
	}

	// write body
	void WriteBody() {
		asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
			[this](std::error_code ec, std::size_t length) {
				if (!ec) {
					// sending message was successful so we do not need it anymore
					m_qMessagesOut.pop_front();
					if (!m_qMessagesOut.empty()) {
						WriteHeader();
					}
				}
				else {
					std::cout << "[" << id << "] Write Body fail.\n";
					m_socket.close();
				}
			});
	}

	void AddToIncomingMessageQueue() {
		if (m_nOwnerType == owner::server)
			m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
		else
			m_q.MessagesIn.push_back({ nullptr, m_msgTemporaryIn });  // As clients only have one conenction anyway

		ReadHeader();  // Register another asio task 
		
	}

protected:
	// Each connection has unique socket
	asio::ip::tcp::socket m_socket;

	// I also want context to be shared throughput the surver connections
	asio::io_context& m_asioContext;

	// all messages to be sent to remote side of connection
	tsqueue<message<T>> m_qMessagesOut;

	// queue in
	// note it is a reference as the "owner" of this connection is expected to provide a queue
	tsqueue<owned_message<T>>& m_qMessagesIn;
	message<T> m_msgTemporaryIn;

	// owner of the connection 
	owner m_nOwnerType = owner::server;
	uint32_t id = 0;
};
