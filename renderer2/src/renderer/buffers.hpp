//
// Created by ENDERZOMBI102 on 26/06/2023.
//

#pragma once

#include <array>
#include <initializer_list>
#include <vector>

#include "../util/stringable.hpp"
#include "glad/glad.h"


class ElementArrayBuffer : public ToString {
	GLuint _handle{0};
	bool hasData{ false };
	int _count;
public:
	ElementArrayBuffer( std::initializer_list<int> indicies );
	~ElementArrayBuffer();

	auto update( std::initializer_list<int> indicies ) -> void;
	template<size_t count>
	auto update( std::array<int, count> indicies ) -> void;
	auto update( const std::vector<int>& indicies ) -> void;

	auto bind() -> void;

	auto getElementCount() -> int;
	auto isReady() -> bool;

	auto toString() -> std::string override;
};
