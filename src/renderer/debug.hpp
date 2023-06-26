//
// Created by ENDERZOMBI102 on 23/06/2023.
//

#pragma once


namespace renderer::Debug {
	auto attach( const GLFWwindow* window ) -> void;

	auto update( char* file, char* func, int line ) -> void;
}
