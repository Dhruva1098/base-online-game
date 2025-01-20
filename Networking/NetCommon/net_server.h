#pragma once
#include "net_thread_safe_queue.h"
#include "net_connection.h"
#include "net_common.h"
#include "net_message.h"

template<typename T>
class server_interface {
public:
	server_interface(uint16_t port) 
		: m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)){

	}
	virtual ~server_interface() {
		Stop();
	}

	bool Start() {
		try {
			WaitForClientConnection();

			m_threadContext = std::thread([this]() { m_asioContext.run(); }); // order is important because we need to keep it alive

		}
		catch (std::exception& e) {
			std::cerr << "[SERVER] exception occured: " << e.what() << "\n";
			return false;
		}
		std::cout << "[SERVER] Started" << "\n";
		return true;
	}

	void Stop() {
		// close context
		m_asioContext.stop();
		
		// tidy up thread
		if (m_threadContext.joinable()) m_threadContext.join();
		
		std::cout << "[SERVER] Stopped" << "\n";
	}
public:
	// async (performed by asio)
	void WaitForClientConnection() {
		m_asioAcceptor.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket) {
				if (!ec) {
					std::cout << "[SERVER] New connection established: " << socket.remote_endpoint() << "\n";
					std::shared_ptr<connection<T>> newconn =
						std::make_shared<connection<T>>(connection<T>::owner::server,
							m_asioContext, std::move(socket), m_qMessagesIn);

					// user can deny connection
					if (OnClientConnect(newconn)) {
						m_deqConnections.push_back(std::move(newconn));
						m_deqConnections.back()->ConnectToClient(nIDCounter++);
						std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
					}
					else {
						std::cout << "[----] Connection denied\n";
					}
				}
				else {
					std::cerr << "[SERVER] New connection error: " << ec.message() << "\n";
				}
				// wait for another connection - re-register another async task
				WaitForClientConnection();

			}
		);
	}
	// send message to a specific client
	void MessageClient(std::shared_ptr<connection<T>> client, const message<T>& msg) {
		if (client && client->IsConnectrd()) {
			client->Send(msg);
		}  // now we do not know if client has disconnected because of TCP limitations, so if he has disconnected
			// we remove him

		else {
			OnClientDisconnect(client);
			m_deqConnections.erase(
				std::remove(m_deqConnections.begin(), m_deqConnections.end(), client));
		}

	}
	// send message to all clients
	void MessageAllClients(const message<T>& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr) {
		bool bInvalidClientExists = false;
		for (auto& client : m_deqConnections) {
			if (client && client->IsConnected()) {
				if (client != pIgnoreClient)
					client->Send(msg);
			}
			else {
				// assume client is disconnected
				OnClientDisconnect(client);
				client.reset();
				bInvalidClientExists = true;	
			}
		}
		// Little optimisation, als i do not want to change deque while iterating through it
		if (bInvalidClientExists)
			m_deqConnections.erase(
				std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullptr));
	}
	void Update(size_t nMaxMessages = -1, bool bWait = false) {
		if (bWait) m_qMessagesIn.wait();

		size_t nMessageCount = 0;
		while (nMessageCount < nMaxMessages && !m_qMessagesIn.empty()) {
			// grab front message
			auto msg = m_qMessagesIn.pop_front();

			// pass it to message handler
			OnMessage(msg.remote, msg.msg);
			nMessageCount++;
		}
	}

protected:
	// when a client connects, can veto connection by returning false
	virtual bool OnClientConnect(std::shared_ptr<connection<T>> client) {
		return false;
	}
	// client disconnects
	virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client) {

	}
	// when message arrives
	virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T>& msg) {

	}
protected:
	// thread sage queue for incoming messages
	tsqueue<owned_message<T>> m_qMessagesIn;

	// container for all active connections
	std::deque<std::shared_ptr<connection<T>>> m_deqConnections;
	
	// order of init of asio
	asio::io_context m_asioContext;
	std::thread m_threadContext;

	asio::ip::tcp::acceptor m_asioAcceptor;
	
	// every client is identified using a unique id
	uint16_t nIDCounter = 10000; // because we do not want to send ip of others to clients
};
