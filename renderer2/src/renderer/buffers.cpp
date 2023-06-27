//
// Created by ENDERZOMBI102 on 27/06/2023.
//

#include "buffers.hpp"
#include <initializer_list>

#include "glad/glad.h"
#include "Renderer.hpp"

ElementArrayBuffer::ElementArrayBuffer( std::initializer_list<int> indicies ) : _count( static_cast<int>( indicies.size() ) ) {
	GL_CALL( glGenBuffers( 1, &this->_handle ) );
	this->bind();
//	GL_CALL( glBufferData( GL_ELEMENT_ARRAY_BUFFER, 200, GL_DYNAMIC_DRAW ) );
	if ( this->_count )
		this->update( indicies );
}

ElementArrayBuffer::~ElementArrayBuffer() {
	glDeleteBuffers( 1, &this->_handle );
}

auto ElementArrayBuffer::update( std::initializer_list<int> indicies ) -> void {

}
auto ElementArrayBuffer::update( const std::vector<int>& indicies ) -> void {

}
template< size_t count >
auto ElementArrayBuffer::update( std::array<int, count> indicies ) -> void {

}

auto ElementArrayBuffer::bind() -> void {

}

auto ElementArrayBuffer::getElementCount() -> int {
	return 0;
}

auto ElementArrayBuffer::toString() -> std::string {
	return "";
}
