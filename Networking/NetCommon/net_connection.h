#pragma once

#include "net_common.h"
#include "net_thread_safe_queue.h"
#include "net_message.h"

template<typename T>
class connection : public std::enable_shared_from_this<connection<T>> {
public:
	connection() {}

	virtual ~connection() {}

public:
	bool ConnectToServer();  // can only be called by client (duh)
	bool Disconnect();
	bool IsConnected() const;

public:
	bool Send(const message<T>& msg);

protected:
	// Each connection has unique socket
	asio::ip::tcp::socket m_socket;

	// I also want context to be shared throughput the surver connections
	asio::io_context& m_asioContext;

	// all messages to be sent to remote side of connection
	tsqueue<message<T>> m_qMessagesOut;

	// queue in
	// note it is a reference as the "owner" of this connection is expected to provide a queue
	tsqueue<owned_message<T>> m_qMessagesIn;
};
