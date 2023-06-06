/***************************************************************
 * Name:      titanicMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-04-30
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/
#include <filesystem>
#include <iostream>

#include "ShipSandbox.hpp"

int main() {
	// make sure we're running in `$REPO/run`
	if ( std::filesystem::current_path().filename() != "run" ) {
		std::cerr << "Game wasn't run in `$REPO/run` directory, aborting." << std::endl;
		return 1;
	}
	shipSandbox = new ShipSandbox{};
	return shipSandbox->mainLoop();
}
