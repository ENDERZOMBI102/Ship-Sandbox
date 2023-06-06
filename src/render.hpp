#pragma once

#include "vec.hpp"

namespace render {
	void triangle( vec2f a, vec2f b, vec2f c );
	void triangle( vec2f posa, vec2f posb, vec2f posc, vec3f cola, vec3f colb, vec3f colc );
	void box( vec2f bottomleft, vec2f topright );
	void setColour( vec3f color );
}// namespace render
