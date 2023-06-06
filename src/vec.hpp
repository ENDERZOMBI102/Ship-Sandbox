#pragma once

#include <string>


struct vec2f {
	float x;
	float y;

	vec2f operator+( const vec2f& rhs ) const;
	vec2f operator-( const vec2f& rhs ) const;
	vec2f operator*( float rhs ) const;
	vec2f operator/( float rhs ) const;
	vec2f& operator+=( const vec2f& rhs );
	vec2f& operator-=( const vec2f& rhs );
	vec2f& operator*=( float rhs );
	vec2f& operator/=( float rhs );
	bool operator==( const vec2f& rhs ) const;
	bool operator<( const vec2f& rhs ) const;  // (lexicographic comparison only)
	[[nodiscard]] float dot( const vec2f& rhs ) const;
	[[nodiscard]] float length() const;
	[[nodiscard]] vec2f normalise() const;
	[[nodiscard]] std::string toString() const;

	explicit inline vec2f( float _x = 0, float _y = 0 )  {
		x = _x;
		y = _y;
	}
};

template<>
struct std::hash<vec2f> {
	size_t operator()( const vec2f& vec ) const {
		auto hasher = hash<float>();
		return hasher( vec.x ) + hasher( vec.y );
	}
};

struct vec3f {
	float x;
	float y;
	float z;

	vec3f operator+( const vec3f& rhs ) const;
	vec3f operator-( const vec3f& rhs ) const;
	vec3f operator*( float rhs ) const;
	vec3f operator/( float rhs ) const;
	vec3f& operator+=( const vec3f& rhs );
	vec3f& operator-=( const vec3f& rhs );
	vec3f& operator*=( float rhs );
	vec3f& operator/=( float rhs );
	bool operator==( const vec3f& rhs ) const;
	bool operator<( const vec3f& rhs ) const;  // (lexicographic comparison only)
	[[nodiscard]] float dot( const vec3f& rhs ) const;
	[[nodiscard]] float length() const;
	[[nodiscard]] vec3f normalise() const;
	[[nodiscard]] std::string toString() const;

	explicit inline vec3f( float _x = 0, float _y = 0, float _z = 0 ) {
		x = _x;
		y = _y;
		z = _z;
	}
};

template<>
struct std::hash<vec3f> {
	size_t operator()( const vec3f& vec ) const {
		auto hasher = hash<float>();
		return hasher( vec.x ) + hasher( vec.y ) + hasher( vec.z );
	}
};
