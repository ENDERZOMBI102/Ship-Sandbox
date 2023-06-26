//
// Created by ENDERZOMBI102 on 23/06/2023.
//

#include "glad/glad.h"
#include <string>

#include "GLFW/glfw3.h"
#include "debug.hpp"

namespace renderer::Debug {
	struct Message {
		GLenum source;
		GLenum type;
		GLuint id;
		GLenum severity;
		std::string* message;
	};
	Message lastMessage{ 0, 0, 0, 0, nullptr };

	static auto logMessage( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam ) -> void {
		lastMessage.source = source;
		lastMessage.type = type;
		lastMessage.id = id;
		lastMessage.severity = severity;
		lastMessage.message = new std::string( message, length );
	}

	auto attach( const GLFWwindow* window ) -> void {
		glEnable( GL_ARB_debug_output );
		glDebugMessageCallbackARB( logMessage, window );
	}

	auto update( char* file, char* func, int line ) -> void {
		if ( lastMessage.message == nullptr )
			return;

		// TODO: THIS
	}
}
