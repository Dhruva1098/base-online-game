#include <network_system.h>

#include "../GameServer/game_common.h"
#include "olcPixelGameEngine.h"
#include "extensions/olcPGEX_TransformedView.h"

class Game : public olc::PixelGameEngine, client_interface<GameMsg> {
public:
	Game() {
		sAppName = "GameClient";
	}

public:
	bool OnUserCreate() override {
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override {
		return true;
	}
};

int main() {
	return 0;	
}