/***************************************************************
 * Name:      titanicMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-04-30
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#include "ShipSandbox.hpp"

int main() {
	shipSandbox = new ShipSandbox{};
	return shipSandbox->mainLoop();
}
