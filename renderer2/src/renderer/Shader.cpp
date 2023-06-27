//
// Created by ENDERZOMBI102 on 27/06/2023.
//

#include "spdlog/spdlog.h"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"

auto createShader( const char* code, GLenum type ) -> GLuint {
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
	throw std::exception();
}

renderer::Shader::Shader( const char* vertex, const char* fragment ) {
	auto vertShader = createShader( vertex, GL_VERTEX_SHADER );
	auto fragShader = createShader( fragment, GL_FRAGMENT_SHADER );

	this->_handle = glCreateProgram();
	glAttachShader( this->_handle, vertShader );
	glAttachShader( this->_handle, fragShader );
	glLinkProgram( this->_handle );
	glDeleteShader( vertShader );
	glDeleteShader( fragShader );

	int success;
	glGetProgramiv( this->_handle, GL_LINK_STATUS, &success );
	if (! success ) {
		char infoLog[ 512 ];
		glGetProgramInfoLog( this->_handle, 512, nullptr, infoLog );
		spdlog::critical( "Failed to link program:\n{}", infoLog );
		throw std::exception();
	}
}

renderer::Shader::~Shader() {
	glDeleteProgram( this->_handle );
}

auto renderer::Shader::bind() const -> void {
	glUseProgram( this->_handle );
}

void renderer::Shader::setBool( const std::string& name, bool value ) const {
	glUniform1i( glGetUniformLocation( this->_handle, name.c_str() ), static_cast<int>( value ) );
}

void renderer::Shader::setInt( const std::string& name, int value ) const {
	glUniform1i( glGetUniformLocation( this->_handle, name.c_str() ), value );
}

void renderer::Shader::setFloat( const std::string& name, float value ) const {
	glUniform1f( glGetUniformLocation( this->_handle, name.c_str() ), value );
}

auto renderer::Shader::setMat4( const std::string& name, glm::mat4 value ) const -> void {
	glUniformMatrix4fv( glGetUniformLocation( this->_handle, name.c_str() ), 1, GL_FALSE, glm::value_ptr( value ) );
}
