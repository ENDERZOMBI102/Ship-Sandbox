//
// Created by ENDERZOMBI102 on 05/06/2023.
//
#include <IL/il.h>
#include <IL/ilu.h>
#include <RmlUi/Debugger.h>
#include <glad/glad.h>
#include <cmath>
#include <iostream>


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
	if (! glfwInit() )
		return -1;

	this->window = glfwCreateWindow( 1024, 768, "Sinking Simulator", nullptr, nullptr );
	glfwSwapInterval( 1 );
	glfwSetScrollCallback( this->window, ShipSandbox::onScroll );
	glfwSetCursorPosCallback( this->window, ShipSandbox::onCursorPos );
	glfwSetKeyCallback( this->window, ShipSandbox::onKey );
	glfwSetMouseButtonCallback( this->window, ShipSandbox::onMouseBtn );
	double lasttime = glfwGetTime();
	int nframes = 0;

	glfwMakeContextCurrent( this->window );

	/* Initialize glad */
	if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) ) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Rml::SetSystemInterface( &this->systemInterface );
	Rml::SetRenderInterface( &this->renderInterface );
	Rml::Initialise();

	if (! (this->rmlContext = Rml::CreateContext( "default", Rml::Vector2i( 1024, 768 ) )) ) {
		Rml::Shutdown();
		return  -1;
	}

	Rml::Debugger::Initialise( this->rmlContext );

	Rml::LoadFontFace( "C:/Windows/Fonts/arial.ttf" );

	auto document = this->rmlContext->LoadDocument( "data/view/main.rml" );

	if (! document ) {
		Rml::Log::Message( Rml::Log::LT_ERROR, "Failed to load document!" );
		Rml::RemoveContext( "default" );
		Rml::Shutdown();
		glfwTerminate();
		return 1;
	}

	document->Show();

	gm.loadShip( "ship.png" );

	while ( !glfwWindowShouldClose( this->window ) ) {
		nframes++;
		if ( glfwGetTime() - lasttime > 1.0 ) {
			lasttime = glfwGetTime();
			glfwSetWindowTitle( this->window, ( "Sinking Simulator - " + this->gm.lastFilename + " (" + std::to_string( nframes ) + " FPS)" ).c_str() );
			nframes = 0;
		}

		this->handleMouse();

		this->rmlContext->Update();
		this->gm.update();

		this->beginFrame();
			this->gm.render();
			this->renderInterface.SetViewport( this->gm.canvaswidth, this->gm.canvasheight );
			this->renderInterface.BeginFrame();
//				this->rmlContext->Render();
			this->renderInterface.EndFrame();
		this->endFrame();

		glfwPollEvents();
	}
	document->Close();
	Rml::RemoveContext( "default" );
	Rml::Shutdown();
	glfwTerminate();
	return 0;
}

auto ShipSandbox::onScroll( GLFWwindow* /*window*/, double x, double y ) -> void {
	shipSandbox->scroll_delta += static_cast<int>( y );
	RmlGLFW::ProcessScrollCallback( shipSandbox->rmlContext, x, 0 );
}

auto ShipSandbox::onCursorPos( GLFWwindow* /*window*/, double xPos, double yPos ) -> void {
	shipSandbox->gm.mouse.x = xPos;
	shipSandbox->gm.mouse.y = yPos;
	RmlGLFW::ProcessCursorPosCallback( shipSandbox->rmlContext, xPos, yPos, 0 );
}

auto ShipSandbox::onKey( GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods ) -> void {
	// Toggle the debugger with a key binding.
	if ( key == GLFW_KEY_F8 && action == GLFW_PRESS )
		Rml::Debugger::SetVisible(! Rml::Debugger::IsVisible() );
	RmlGLFW::ProcessKeyCallback( shipSandbox->rmlContext, key, action, mods );
}

auto ShipSandbox::onMouseBtn( GLFWwindow* /*window*/, int button, int action, int mods ) -> void {
	RmlGLFW::ProcessMouseButtonCallback( shipSandbox->rmlContext, button, action, mods );
}

ShipSandbox* shipSandbox;
