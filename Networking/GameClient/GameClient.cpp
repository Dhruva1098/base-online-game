#include "../GameServer/game_common.h"


class Game : public olc::PixelGameEngine, client_interface<GameMsg> {  // Multiple inheritance boooooo
public:
	Game() {
		sAppName = "GameClient";
	}

private:
	olc::TileTransformedView tv;
	std::string	sWorldMap =
		"################################"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..........####...####.........#"
		"#..........#.........#.........#"
		"#..........#.........#.........#"
		"#..........#.........#.........#"
		"#..........##############......#"
		"#..............................#"
		"#..................#.#.#.#.....#"
		"#..............................#"
		"#..................#.#.#.#.....#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"################################";
	
	olc::vi2d vWorldSize = { 32,32 };

public:
	bool OnUserCreate() override {
		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 8,8 });
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override {
		// Handle zoom
		if (GetMouse(1).bPressed) tv.StartPan(GetMousePos());
		if (GetMouse(1).bHeld) tv.UpdatePan(GetMousePos());
		if (GetMouse(1).bReleased) tv.EndPan(GetMousePos());
		if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(1.5f, GetMousePos());
		if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.75f, GetMousePos());

		// Clear world
		Clear(olc::BLACK);

		// DRaw World
		olc::vi2d vTL = tv.GetTopLeftTile().max({ 0,0 });
		olc::vi2d vBR = tv.GetBottomRightTile().min(vWorldSize);
		olc::vi2d vTile;
		for(vTile.y = vTL.y; vTile.y < vBR.y; vTile.y++)
			for (vTile.x = vTL.x; vTile.x < vBR.x; vTile.x++) {
				if (sWorldMap[vTile.y * vWorldSize.x + vTile.x] == '#') {
					tv.DrawRect(vTile, { 1.0f,1.0f });
					tv.DrawRect(olc::vf2d(vTile) + olc::vf2d(0.1f, 0.1f), { 0.8f, 0.8f });
				}
			}

		return true;
	}
};	

int main() {
	Game demo;
	if (demo.Construct(640, 640, 2, 2))
		demo.Start();
	return 0;	
}