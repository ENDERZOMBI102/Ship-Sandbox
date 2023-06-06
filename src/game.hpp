#pragma once

#include "phys.h"
#include <vector>


class Game {
	std::vector< Material* > materials;
public:
	struct Mouse {
		bool ldown;
		bool rdown;
		double x, y;
		double lastx, lasty;

		[[nodiscard]] auto getPosVec() const -> vec2f;
		[[nodiscard]] auto getLastPosVec() const -> vec2f;
	};
	enum class ToolType { SMASH = 0, GRAB };
public:
	Mouse mouse{};
	ToolType tool{};

	double strength;
	double buoyancy;
	double waveheight;
	double waterpressure;
	double seadepth;
	bool showstress;
	bool quickwaterfix;
	bool xraymode;

	bool running;

	float zoomsize;
	float camx{};
	float camy{};
	int canvaswidth{}, canvasheight{};
	void loadShip( const std::string& filename );
	void loadDepth( const std::string& filename );
	void assertSettings() const;
	[[nodiscard]] vec2f screen2world( vec2f ) const;

	std::string lastFilename;
	double* oceandepthbuffer{};

	phys::world* wld;
	Game();
	void render() const;
	void update();
};
