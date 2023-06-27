//
// Created by ENDERZOMBI102 on 23/06/2023.
//

#include "glad/glad.h"
#include <string>

#include "GLFW/glfw3.h"
#include "debug.hpp"
#include "spdlog/spdlog.h"

namespace renderer::Debug {
	struct Message {
		GLenum source;
		GLenum type;
		GLuint id;
		GLenum severity;
		std::string* text;
	};
	Message lastMessage{ 0, 0, 0, 0, nullptr };

	static auto logMessage( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) -> void {
		lastMessage.source = source;
		lastMessage.type = type;
		lastMessage.id = id;
		lastMessage.severity = severity;
		lastMessage.text = new std::string( message, length );
		spdlog::info( "Something: {}", std::string( message, length ) );
	}

	auto attach( const GLFWwindow* window ) -> void {
		glEnable( GL_KHR_debug );
		glDebugMessageCallback( logMessage, window );
	}

	auto update( const char* file, const char* func, int line ) -> void {
		if ( lastMessage.text == nullptr )
			return;

		switch ( lastMessage.severity ) {
			case GL_DEBUG_SEVERITY_HIGH:
				spdlog::error( "[OpenGL Debug HIGH] in `{}:{}` at line {}: {}", file, func, line, *lastMessage.text );
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				spdlog::warn( "[OpenGL Debug MEDIUM] in `{}:{}` at line {}: {}", file, func, line, *lastMessage.text );
				break;
			case GL_DEBUG_SEVERITY_LOW:
				spdlog::info( "[OpenGL Debug LOW] in `{}:{}` at line {}: {}", file, func, line, *lastMessage.text );
				break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				spdlog::error( "Invalid severity: {}", lastMessage.severity );
				break;
		}
		lastMessage.text = nullptr;
	}
}// namespace renderer::Debug
