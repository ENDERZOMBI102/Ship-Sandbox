//
// Created by ENDERZOMBI102 on 05/06/2023.
//

#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <RmlUi/Core.h>

#include "RmlBackend/RmlUi_Platform_GLFW.h"
#include "RmlBackend/RmlUi_Renderer_GL2.h"
#include "game.hpp"

class ShipSandbox {
	GLFWwindow* window{};
	Game gm{};
	SystemInterface_GLFW systemInterface{};
	RenderInterface_GL2 renderInterface{};
	Rml::Context* rmlContext{};
	Rml::ElementDocument* rmlDoc{};
	int scroll_delta{0};
public:
	auto beginFrame()-> void;
	auto endFrame() -> void;
	auto handleMouse() -> void;
	auto mainLoop() -> int;
private:
	static auto onScroll( GLFWwindow* window, double x, double y ) -> void;
	static auto onCursorPos( GLFWwindow* window, double xPos, double yPos ) -> void;
	static auto onKey( GLFWwindow* window, int key, int scancode, int action, int mods ) -> void;
	static auto onMouseBtn( GLFWwindow* window, int button, int action, int mods ) -> void;
};

extern ShipSandbox* shipSandbox;
