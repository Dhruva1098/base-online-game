#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGE_TRANSFORMEDVIEW
#include "extensions/olcPGEX_TransformedView.h"

class CircleVRect : public olc::PixelGameEngine {
public:
	CircleVRect() {
		sAppName = "Circle vs rectangle";
	}

private:
	olc::TileTransformedView tv;

private:
	bool OnUserCreate() override {  // called at start
		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 32,32 });
		return true;
	}

	bool OnUserUpdate(float fElapsedTIme) override {  // called every frame
		Clear(olc::VERY_DARK_BLUE);
		tv.DrawCircle({0,0}, 20);
		return true; 
	}
};

int main() {
	CircleVRect demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;

}