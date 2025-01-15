#include <iostream>
#include <olc.h>

enum class CustomMsgTypes : uint32_t {
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomServer : public server_interface<CustomMsgTypes> {
public:
	CustomServer(uint16_t nPort) : server_interface<CustomMsgTypes>(nPort) {

	}
protected:
	virtual bool OnClientConnect(std::shared_ptr<connection<CustomMsgTypes>> client) {
		return true;
	}

	virtual void OnClientDisconnect(std::shared_ptr<connection<CustomMsgTypes>> client) {

	}
	virtual void OnMessage(std::shared_ptr<connection<CustomMsgTypes>> client, message<CustomMsgTypes> msg) {

	}

};

int main() {
	CustomServer server(60000);
	server.Start();

	while (1) {
		server.Update();
	}
	return 0;
}