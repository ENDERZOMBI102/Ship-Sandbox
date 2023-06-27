//
// Created by ENDERZOMBI102 on 27/06/2023.
//

#pragma once

#include "stringable.hpp"
#include <sstream>
#include <string>

auto ToString::toString() -> std::string {
	return std::string( typeid( this ).name() ) + "@" + std::to_string( reinterpret_cast<size_t>( this ) );
}

