#pragma once

#include "vec.hpp"
#include "json/json.h"
#include <string>

class Material {
public:
    std::string name;
    float strength;
    float mass;
    vec3f color;
    bool isHull;
public:
	explicit Material( const Json::Value& root );
};
