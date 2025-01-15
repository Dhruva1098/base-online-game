#include <iostream>
#include "net_client.h"

enum class CustomMsgTypes : uint32_t {
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomServer : public server_interface<CustomMsgTypes>
