/***************************************************************
 * Name:      titanicMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-04-30
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#include "RmlUi/Core/Core.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "RmlBackend/RmlUi_Platform_GLFW.h"
#include "RmlBackend/RmlUi_Renderer_GL2.h"
#include "game.h"
#include "glad/glad.h"
#include "util.h"
#include <RmlUi/Core.h>
#include <RmlUi/Core/SystemInterface.h>
#include <cmath>
#include <iostream>

int scroll_delta = 0;

//   GGGGG  RRRR        A     PPPP     H     H  IIIIIII    CCC      SSS
//  GG      R   RR     A A    P   PP   H     H     I      CC CC   SS   SS
// GG       R    RR   A   A   P    PP  H     H     I     CC    C  S
// G        R   RR   A     A  P   PP   H     H     I     C        SS
// G        RRRR     AAAAAAA  PPPP     HHHHHHH     I     C          SSS
// G  GGGG  R RR     A     A  P        H     H     I     C             SS
// GG    G  R   R    A     A  P        H     H     I     CC    C        S
//  GG  GG  R    R   A     A  P        H     H     I      CC CC   SS   SS
//   GGGG   R     R  A     A  P        H     H  IIIIIII    CCC      SSS

void initgl( GLFWwindow *window, game *gm )
{
	// Set the context, clear the canvas and set up all the matrices.
	glfwGetWindowSize( window, &( gm->canvaswidth ), &( gm->canvasheight ) );
	glViewport( 0, 0, gm->canvaswidth, gm->canvasheight );
	glClearColor( 0.529, 0.808, 0.980, 1 );//(cornflower blue)
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	float halfheight = gm->zoomsize;
	float halfwidth = ( float ) gm->canvaswidth / gm->canvasheight * halfheight;
	glFrustum( -halfwidth, halfwidth, -halfheight, halfheight, 1, 1000 );
	glTranslatef( -gm->camx, -gm->camy, 0 );

	glEnable( GL_LINE_SMOOTH );
	glHint( GL_LINE_SMOOTH, GL_NICEST );
	glEnable( GL_POINT_SMOOTH );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glPointSize( 0.15f * gm->canvasheight / gm->zoomsize );
	glLineWidth( 0.1f * gm->canvasheight / gm->zoomsize );
	glColor3f( 0, 0, 0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void endgl( GLFWwindow *window, game *gm )
{
	// Flush all the draw operations and flip the back buffer onto the screen.
	glFlush();
	glfwSwapBuffers( window );
}

/*void titanicFrame::OnMenuItemLoadSelected(wxCommandEvent& event)
{
    if (dlgOpen->ShowModal() == wxID_OK)
    {
        std::string filename = dlgOpen->GetPath().ToStdString();
        delete gm.wld;
        gm.wld = new phys::world;
        gm.assertSettings();
        gm.loadShip(filename);
    }
}*/

// M     M    OOO    U     U    SSS    EEEEEEE
// MM   MM   O   O   U     U  SS   SS  E
// M M M M  O     O  U     U  S        E
// M  M  M  O     O  U     U  SS       E
// M     M  O     O  U     U    SSS    EEEE
// M     M  O     O  U     U       SS  E
// M     M  O     O  U     U        S  E
// M     M   O   O    U   U   SS   SS  E
// M     M    OOO      UUU      SSS    EEEEEEE

/*

void titanicFrame::OnMenuReloadSelected(wxCommandEvent& event)
{
        delete gm.wld;
        gm.wld = new phys::world;
        gm.assertSettings();
        gm.loadShip(gm.lastFilename);
}*/

void doInput( GLFWwindow *window, game &gm, SystemInterface_GLFW &systemInterface ) {

	double xd, yd;
	glfwGetCursorPos( window, &xd, &yd );
	gm.mouse.x = xd;
	gm.mouse.y = yd;
	gm.mouse.ldown = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
	gm.mouse.rdown = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
	if ( scroll_delta != 0 ) {
		gm.zoomsize *= pow( 0.85, scroll_delta );
		scroll_delta = 0;
	}
}

void scrollCallback( GLFWwindow *window, double x, double y ) {
	scroll_delta += y;
}

int main()
{
	ilInit();
	iluInit();
	if ( glfwInit() == -1 )
		return -1;

	GLFWwindow *window = glfwCreateWindow( 1024, 768, "Sinking Simulator", NULL, NULL );
	glfwSwapInterval( 1 );
	glfwSetScrollCallback( window, scrollCallback );
	double lasttime = glfwGetTime();
	int nframes = 0;

	glfwMakeContextCurrent( window );

	/* Initialize glad */
	if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	SystemInterface_GLFW systemInterface;
	RenderInterface_GL2 renderInterface;

	Rml::SetSystemInterface( &systemInterface );
	Rml::SetRenderInterface( &renderInterface );
	Rml::Initialise();
	Rml::Log::Initialise();
	Rml::LoadFontFace( "C:/Windows/Fonts/arial.ttf" );

	auto context = Rml::CreateContext( "default", Rml::Vector2i( 1024, 768) );
	auto document = context->LoadDocument( "data/view/main.xml" );

	if (! document ) {
		Rml::Log::Message( Rml::Log::LT_ERROR, "Failed to load document!" );
		Rml::RemoveContext( "default" );
		Rml::Shutdown();
		glfwTerminate();
		return 1;
	}

	document->Show();

    game gm;
    gm.loadShip("ship.png");

    bool running = true;
    while (running && !glfwWindowShouldClose(window))
    {
        nframes++;
        if (glfwGetTime() - lasttime > 1.0)
        {
            lasttime = glfwGetTime();
            glfwSetWindowTitle(window, ("Sinking Simulator - " + gm.lastFilename +  " (" + std::to_string(nframes) + " FPS)").c_str());
            nframes = 0;
        }
		context->Update();

//		// Submit input events before the call to Context::Update().
//		if (my_input->MouseMoved())
//			context->ProcessMouseMove(mouse_pos.x, mouse_pos.y, 0);
//
//		// Toggle the debugger with a key binding.
//		if (my_input->KeyPressed(KEY_F8))
//			Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());

		doInput( window, gm, systemInterface );
        gm.update();
        initgl(window, &gm);
        gm.render();
		renderInterface.SetViewport(  gm.canvaswidth, gm.canvasheight  );
		renderInterface.BeginFrame();
		context->Render();
		renderInterface.EndFrame();
		endgl(window, &gm);

        glfwPollEvents();
    }
	document->Close();
	Rml::RemoveContext( "default" );
	Rml::Shutdown();
    glfwTerminate();
    return 0;
}
