//
// Created by ENDERZOMBI102 on 27/06/2023.
//

#pragma once

#include <string>

#include "glm/glm.hpp"
#include "glad/glad.h"

namespace renderer {
	class Shader {
		GLuint _handle;
	public:
		Shader( const char* vertex, const char* fragment );
		~Shader();

		auto bind() const -> void;

		auto setBool( const std::string& name, bool value ) const -> void;
		auto setInt( const std::string& name, int value ) const -> void;
		auto setFloat( const std::string& name, float value ) const -> void;
		auto setMat4( const std::string& name, glm::mat4 value ) const -> void;
	};
}// namespace renderer
