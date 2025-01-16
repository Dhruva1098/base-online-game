#include <iostream>
#include "olc.h"

enum class CustomMsgTypes : uint32_t {
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomClient : public client_interface<CustomMsgTypes> {

};

int main() {
	CustomClient c;
	c.Connect("127.0.0.1", 60000);
	return 0;
}
