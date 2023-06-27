//
// Created by ENDERZOMBI102 on 27/06/2023.
//

#include <format>

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "../src/renderer/Renderer.hpp"
#include "../src/renderer/Shader.hpp"
#include "../src/renderer/buffers.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"


auto onScroll( GLFWwindow* window, double xOffset, double yOffset ) -> void { }
auto onCursorPos( GLFWwindow* window, double xPos, double yPos ) -> void { }
auto onKey( GLFWwindow* window, int key, int scancode, int action, int mods ) -> void { }
auto onMouseBtn( GLFWwindow* window, int button, int action, int mods ) -> void { }


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

		uniform mat4 transform;

		out vec4 vertexColor;

		void main() {
			gl_Position = transform * vec4( iPos, 1.0 );
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

	renderer::Shader program{ vxShader, fxShader };

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
	renderer::VertexArray vao{};
	renderer::VertexBuffer vbo{ GL_STATIC_DRAW };
	renderer::ElementArrayBuffer ebo{ GL_STATIC_DRAW };

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	vao.bind();

	vbo.data( sizeof(vertices), vertices );

	ebo.data( sizeof(indices), indices );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>( 0 ) );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>( 3 * sizeof(float) ) );
	glEnableVertexAttribArray( 1 );

	// --- MAIN LOOP ---
	glfwShowWindow( window );
	glfwSwapInterval( 1 );
	while ( !glfwWindowShouldClose( window ) ) {
		nframes++;
		if ( glfwGetTime() - lastTime > 1.0 ) {
			lastTime = glfwGetTime();
			glfwSetWindowTitle( window, std::format( "Renderer 2 - {} FPS", nframes ).c_str() );
			spdlog::info( "time: {}", lastTime );
			nframes = 0;
		}

		// --- RENDER ---
		glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		glm::mat4 trans{ 1.0f };
		trans = glm::rotate( trans, static_cast<float>( lastTime ), glm::vec3{ 1.0f, 0.0f, 0.0f } );

		program.bind();
		program.setMat4( "transform", trans );

		vao.bind();
		glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );

		glfwSwapBuffers( window );
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}