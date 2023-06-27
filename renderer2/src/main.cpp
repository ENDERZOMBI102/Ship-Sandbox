//
// Created by ENDERZOMBI102 on 27/06/2023.
//

#include <format>

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "glad/glad.h"
#include "renderer/Renderer.hpp"


auto onScroll( GLFWwindow* window, double xOffset, double yOffset ) -> void { }
auto onCursorPos( GLFWwindow* window, double xPos, double yPos ) -> void { }
auto onKey( GLFWwindow* window, int key, int scancode, int action, int mods ) -> void { }
auto onMouseBtn( GLFWwindow* window, int button, int action, int mods ) -> void { }


auto createShader( const char* code, GLenum type ) -> std::optional<GLuint> {
	auto shader = glCreateShader( type );
	glShaderSource( shader, 1, &code, nullptr );
	glCompileShader( shader );

	int success;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
	if ( success )
		return shader;

	char infoLog[512];
	glGetShaderInfoLog( shader, 512, nullptr, infoLog );
	spdlog::critical( "Failed to compile {} shader:\n{}", type == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog );
	return std::nullopt;
}

auto createProgram( const char* vtx, const char* frg ) -> std::optional<GLuint> {
	auto vertShader = createShader( vtx, GL_VERTEX_SHADER );
	if (! vertShader )
		return std::nullopt;

	auto fragShader = createShader( frg, GL_FRAGMENT_SHADER );
	if (! fragShader )
		return std::nullopt;

	auto program = glCreateProgram();
	glAttachShader( program, *vertShader );
	glAttachShader( program, *fragShader );
	glLinkProgram( program );
	glDeleteShader( *vertShader );
	glDeleteShader( *fragShader );

	int success;
	glGetProgramiv( program, GL_LINK_STATUS, &success );
	if ( success )
		return program;

	char infoLog[512];
	glGetProgramInfoLog( program, 512, nullptr, infoLog );
	spdlog::critical( "Failed to link program:\n{}", infoLog );
	return std::nullopt;
}


int main() {
	auto console = spdlog::stdout_color_mt( "console" );
	auto err_logger = spdlog::stderr_color_mt( "stderr" );
	spdlog::set_pattern( "[%H:%M:%S] [%^%t/%l%$] (%n) %v" );

	if (! Renderer::init() )
		return 1;

	auto window = glfwCreateWindow( 1024, 768, "Renderer 2", nullptr, nullptr );
	glfwMakeContextCurrent( window );
	// setup window properties and callbacks
	glfwSwapInterval( 1 );
	glfwSetScrollCallback( window, onScroll );
	glfwSetCursorPosCallback( window, onCursorPos );
	glfwSetKeyCallback( window, onKey );
	glfwSetMouseButtonCallback( window, onMouseBtn );

	// Initialize glad
	if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) ) {
		spdlog::critical( "Failed to initialize GLAD" )   ;
		return -1;
	}

	double lastTime = glfwGetTime();
	int nframes = 0;

	// --- SHADERS ---
	// language=GLSL
	auto vxShader = R"(
		#version 330 core
		layout (location = 0) in vec3 iPos;
		layout (location = 1) in vec4 iColor;

		out vec4 vertexColor;

		void main() {
			gl_Position = vec4( iPos, 1.0 );
			vertexColor = iColor;
		}
	)";

	// language=GLSL
	auto fxShader = R"(
		#version 330 core
		in vec4 vertexColor;

		out vec4 FragColor;

		void main() {
			FragColor = vertexColor;
		}
	)";

	auto program = createProgram( vxShader, fxShader );
	if (! program )
		return 1;

	// --- DATA ---
	float vertices[] = {
		// positions                         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,   // bottom left
		0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f    // top
	};
	unsigned int indices[] = {
		0, 1, 2,   // first triangle
	};

	// --- BUFFERS ---
	GLuint vao;
	glGenVertexArrays( 1, &vao );

	GLuint vbo;
	glGenBuffers( 1, &vbo );

	GLuint ebo;
	glGenBuffers( 1, &ebo );

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray( vao );

	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>( 0 ) );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>( 3 * sizeof(float) ) );
	glEnableVertexAttribArray( 1 );

	// --- MAIN LOOP ---
	glfwShowWindow( window );
	glfwSwapInterval( 0 );
	while ( !glfwWindowShouldClose( window ) ) {
		nframes++;
		if ( glfwGetTime() - lastTime > 1.0 ) {
			lastTime = glfwGetTime();
			glfwSetWindowTitle( window, std::format( "Renderer 2 - {} FPS", nframes ).c_str() );
			nframes = 0;
		}

		// --- RENDER ---
		glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		glUseProgram( *program );

		glBindVertexArray( vao );
		glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );

		glfwSwapBuffers( window );
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
