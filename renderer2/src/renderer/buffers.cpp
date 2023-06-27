//
// Created by ENDERZOMBI102 on 27/06/2023.
//
#include <format>

#include "glad/glad.h"

#include "Renderer.hpp"
#include "buffers.hpp"


// region -- VERTEX ARRAY ---

renderer::VertexArray::VertexArray() {
	GL_CALL( glGenVertexArrays( 1, &this->_handle ) );
}

renderer::VertexArray::~VertexArray() {
	GL_CALL( glDeleteBuffers( 1, &this->_handle ) );
}

auto renderer::VertexArray::bind() const -> void {
	GL_CALL( glBindVertexArray( this->_handle ) );
}

auto renderer::VertexArray::toString() -> std::string {
	return std::format( "VertexArray{{_handle={}}}", this->_handle );
}
// endregion


// region --- VERTEX BUFFER ---

renderer::VertexBuffer::VertexBuffer( GLenum usage ) {
	GL_CALL( glGenBuffers( 1, &this->_handle ) );
	this->_usage = usage;
}

renderer::VertexBuffer::~VertexBuffer() {
	GL_CALL( glDeleteBuffers( 1, &this->_handle ) );
}

auto renderer::VertexBuffer::bind() const -> void {
	GL_CALL( glBindBuffer( GL_ARRAY_BUFFER, this->_handle ) );
}

auto renderer::VertexBuffer::data( GLsizeiptr size, void* data ) const -> void {
	this->bind();
	GL_CALL( glBufferData( GL_ARRAY_BUFFER, size, data, this->_usage ) );
}

auto renderer::VertexBuffer::toString() -> std::string {
	return std::format( "VertexBuffer{{_handle={}}}", this->_handle );
}
// endregion


// region --- ELEMENT ARRAY BUFFER ---

renderer::ElementArrayBuffer::ElementArrayBuffer( GLenum usage ) {
	GL_CALL( glGenBuffers( 1, &this->_handle ) );
	this->_usage = usage;
}

renderer::ElementArrayBuffer::~ElementArrayBuffer() {
	GL_CALL( glDeleteBuffers( 1, &this->_handle ) );
}

auto renderer::ElementArrayBuffer::bind() const -> void {
	GL_CALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->_handle ) );
}

auto renderer::ElementArrayBuffer::data( GLsizeiptr size, void* data ) const -> void {
	this->bind();
	GL_CALL( glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, data, this->_usage ) );
}

auto renderer::ElementArrayBuffer::toString() -> std::string {
	return std::format( "ElementArrayBuffer{{_handle={}}}", this->_handle );
}
// endregion
