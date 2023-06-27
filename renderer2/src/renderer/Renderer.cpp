//
// Created by ENDERZOMBI102 on 23/06/2023.
//

#include <atomic>
#include <optional>
#include <thread>

#include "spdlog/spdlog.h"

#include "GLFW/glfw3.h"
#include "Renderer.hpp"
#include "debug.hpp"

#define OGL_MAJOR 3
#define OGL_MINOR 3

namespace Renderer {
	static std::atomic<bool> initialized{ false };
	static std::optional<std::thread::id> renderThread{ std::nullopt };

	auto init() -> bool {
		if ( initialized )
			return true;

		initialized = true;
		renderThread = std::this_thread::get_id();
		spdlog::info( "Initializing renderer" );

		int major, minor, revision;
		glfwGetVersion( &major, &minor, &revision );
		spdlog::info( "Running on GLFW {}.{}.{} with OpenGL {}.{}", major, minor, revision, OGL_MAJOR, OGL_MINOR );

		glfwSetErrorCallback( []( int error_code, const char* description ) {
			spdlog::error( "GlfwError: {}: {}", error_code, description );
		} );

		// Initialize GLFW. Most GLFW functions will not work before doing this.
		if (! glfwInit() ) {
			spdlog::critical( "Unable to initialize GLFW!" );
			return false;
		}

		// Configure GLFW
		glfwDefaultWindowHints(); // The current window hints are already the default
		glfwWindowHint( GLFW_VISIBLE              , GLFW_FALSE  ); // The window will stay hidden after creation
		glfwWindowHint( GLFW_RESIZABLE            , GLFW_FALSE  ); // The window will not be resizable
		glfwWindowHint( GLFW_FOCUS_ON_SHOW        , GLFW_TRUE   ); // Focus the window when shown
		glfwWindowHint( GLFW_AUTO_ICONIFY         , GLFW_FALSE  ); // Makes a fullscreen window iconify on focus loss
		glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT , GLFW_TRUE   ); // Enables additional error and performance issue reporting functionality
		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, OGL_MAJOR   ); // Set OpenGL version to 3.3
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, OGL_MINOR   );
		glfwWindowHint( GLFW_OPENGL_PROFILE       , GLFW_OPENGL_CORE_PROFILE ); // Set the OGL Profile to the core one
		glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE   ); // Removes all deprecated functionality from the requested OGL version

		return true;
	}

	auto assertIsOnRenderThread( const char* message ) -> void {
		if ( isOnRenderThread() )
			return;
		throw std::exception( message );
	}

	auto isOnRenderThread() -> bool {
		return std::this_thread::get_id() == renderThread;
	}

	auto isInitialized() -> bool {
		return initialized;
	}

	auto checkErrors( const char* file, const char* func, int line ) -> void {
		renderer::Debug::update( file, func, line );
	}
}// namespace engine::renderer
