//
// Created by ENDERZOMBI102 on 23/06/2023.
//

#pragma once

namespace Renderer {

	/**
	 * Initializes the renderer.<br/>
	 * This will:<br/>
	 *  - Initialize GLFW.<br/>
	 *  - Mark the current thread as the render thread.<br/>
	 *  - Set the required OpenGL version.<br/>
	 *  - Log some information.
	 *  @returns true if initialized, false otherwise.
	 */
	auto init() -> bool;

	/**
	 * Throws an exception if not called on the render thread.
	 * @param text text for the exception.
	 */
	auto assertIsOnRenderThread( const std::string& message );

	/**
	 * Checks whether it's called on the render thread.
	 * @return true if it is, false otherwise.
	 */
	auto isOnRenderThread() -> bool;

	/**
	 * Whether the Renderer was initialized.
	 * @return true if it is, false otherwise.
	 */
	auto isInitialized() -> bool;

	/**
	 * Check for OpenGL errors.
	 * @param file calling file.
	 * @param func calling function.
	 * @param line calling line.
	 */
	auto checkErrors( const char* file, const char* func, int line ) -> void;
}// namespace engine

#define GL_CALL(expr) expr; Renderer::checkErrors( __FILE__, __FUNCTION__, __LINE__ )
