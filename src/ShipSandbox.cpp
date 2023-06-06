//
// Created by ENDERZOMBI102 on 05/06/2023.
//
#include <IL/il.h>
#include <IL/ilu.h>
#include <imgui.h>
#include <glad/glad.h>
#include <cmath>
#include <iostream>


#include "ImBackend/imgui_impl_glfw.h"
#include "ImBackend/imgui_impl_opengl2.h"
#include "ShipSandbox.hpp"

auto ShipSandbox::beginFrame() -> void {
	// Set the context, clear the canvas and set up all the matrices.
	glfwGetWindowSize( this->window, &this->gm.canvaswidth, &this->gm.canvasheight );
	glViewport( 0, 0, this->gm.canvaswidth, this->gm.canvasheight );
	glClearColor( 0.529, 0.808, 0.980, 1 ); // (cornflower blue)
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	float halfheight = this->gm.zoomsize;
	auto halfwidth = static_cast<float>( this->gm.canvaswidth ) / static_cast<float>( this->gm.canvasheight ) * halfheight;
	glFrustum( -halfwidth, halfwidth, -halfheight, halfheight, 1, 1000 );
	glTranslatef( -this->gm.camx, -this->gm.camy, 0 );

	glEnable( GL_LINE_SMOOTH );
	glHint( GL_LINE_SMOOTH, GL_NICEST );
	glEnable( GL_POINT_SMOOTH );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glPointSize( 0.15f * static_cast<float>( this->gm.canvasheight ) / this->gm.zoomsize );
	glLineWidth( 0.1f * static_cast<float>( this->gm.canvasheight ) / this->gm.zoomsize );
	glColor3f( 0, 0, 0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

auto ShipSandbox::endFrame() -> void {
	// Flush all the draw operations and flip the back buffer onto the screen.
	glFlush();
	glfwSwapBuffers( this->window );
}

auto ShipSandbox::handleMouse() -> void {
	this->gm.mouse.ldown = glfwGetMouseButton( this->window, GLFW_MOUSE_BUTTON_LEFT );
	this->gm.mouse.rdown = glfwGetMouseButton( this->window, GLFW_MOUSE_BUTTON_RIGHT );
	if ( this->scroll_delta != 0 ) {
		this->gm.zoomsize *= static_cast<float>( std::pow( 0.85, this->scroll_delta ) );
		this->scroll_delta = 0;
	}
}

auto ShipSandbox::mainLoop() -> int {
	ilInit();
	iluInit();
	if ( glfwInit() == -1 )
		return -1;
	
	this->window = glfwCreateWindow( 1024, 768, "Sinking Simulator", nullptr, nullptr );
	glfwSwapInterval( 1 );
	glfwSetScrollCallback( this->window, ShipSandbox::onScroll );
	glfwSetCursorPosCallback( this->window, ShipSandbox::onCursorPos );
	glfwSetKeyCallback( this->window, ShipSandbox::onKey );
	double lasttime = glfwGetTime();
	int nframes = 0;

	glfwMakeContextCurrent( this->window );

	/* Initialize glad */
	if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) ) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL( window, true );
	ImGui_ImplOpenGL2_Init();

	io.Fonts->AddFontDefault();

	gm.loadShip( "ship.png" );
	bool shown = true;

	while ( !glfwWindowShouldClose( this->window ) ) {
		nframes++;
		if ( glfwGetTime() - lasttime > 1.0 ) {
			lasttime = glfwGetTime();
			glfwSetWindowTitle( this->window, ( "Sinking Simulator - " + this->gm.lastFilename + " (" + std::to_string( nframes ) + " FPS)" ).c_str() );
			nframes = 0;
		}

		this->handleMouse();

		this->gm.update();
		this->beginFrame();
			this->gm.render();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL2_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Another Window", &shown);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					shown = false;
			ImGui::End();

			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		this->endFrame();

		glfwPollEvents();
	}
	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

auto ShipSandbox::onScroll( GLFWwindow* /*window*/, double x, double y ) -> void {
	shipSandbox->scroll_delta += static_cast<int>( y );
}

auto ShipSandbox::onCursorPos( GLFWwindow* /*window*/, double xPos, double yPos ) -> void {
	shipSandbox->gm.mouse.x = xPos;
	shipSandbox->gm.mouse.y = yPos;
}

auto ShipSandbox::onKey( GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods ) -> void {
	(void) 0;
}

ShipSandbox* shipSandbox;
