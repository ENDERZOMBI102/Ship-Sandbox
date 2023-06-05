//
// Created by ENDERZOMBI102 on 05/06/2023.
//

#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <RmlUi/Core.h>

#include "RmlBackend/RmlUi_Platform_GLFW.h"
#include "RmlBackend/RmlUi_Renderer_GL2.h"
#include "game.h"

class ShipSandbox {
	GLFWwindow* window{};
	game gm{};
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
};

extern ShipSandbox* shipSandbox;
