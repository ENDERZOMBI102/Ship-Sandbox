//
// Created by ENDERZOMBI102 on 27/06/2023.
//

#include "buffers.hpp"

#include "glad/glad.h"
#include "Renderer.hpp"

ElementArrayBuffer::ElementArrayBuffer() {
	glGenBuffers( 1, &this->_handle );
	CHECK_ERRORS();
}

ElementArrayBuffer::~ElementArrayBuffer() {
}
