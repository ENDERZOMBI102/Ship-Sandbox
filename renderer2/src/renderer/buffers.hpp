//
// Created by ENDERZOMBI102 on 26/06/2023.
//

#pragma once

#include <array>
#include <initializer_list>
#include <vector>

#include "glad/glad.h"

#include "../util/stringable.hpp"


namespace renderer {
	class VertexArray : public ToString {
		GLuint _handle{0};
	public:
		VertexArray();
		~VertexArray();

		auto bind() const -> void;

		auto toString() -> std::string override;
	};

	class VertexBuffer : public ToString {
		GLuint _handle{0};
		GLenum _usage;
	public:
		explicit VertexBuffer( GLenum usage );
		~VertexBuffer();

		auto bind() const -> void;

		auto data( GLsizeiptr size, void* data ) const -> void;

		auto toString() -> std::string override;
	};

	class ElementArrayBuffer : public ToString {
		GLuint _handle{0};
		GLenum _usage;
	public:
		explicit ElementArrayBuffer( GLenum usage );
		~ElementArrayBuffer();

		auto bind() const -> void;

		auto data( GLsizeiptr size, void* data ) const -> void;

		auto toString() -> std::string override;
	};
}
