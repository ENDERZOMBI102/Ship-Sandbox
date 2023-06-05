#include <iostream>

#include "material.hpp"


int hex2int( const std::string& str ) {
	std::stringstream ss;
	ss << std::hex << str;
	int x;
	ss >> x;
	return x;
}

vec3f hex2Color( std::string str ) {  //  e.g. "#00FF00";
	if ( str[ 0 ] == '#' )
		str = str.substr( 1 );
	if ( str.length() != 6 )
		std::cout << "Error: badly formed hex color value!\n";
	return {
		static_cast<float>( hex2int( str.substr( 0, 2 ) ) ) / 255.f,
		static_cast<float>( hex2int( str.substr( 2, 2 ) ) ) / 255.f,
		static_cast<float>( hex2int( str.substr( 4, 2 ) ) ) / 255.f
	};
}

Material::Material( const Json::Value& root ) {
	this->name = root.get( "name", "Unspecified" ).asString();
	this->mass = root.get( "mass", 1 ).asFloat();
	this->strength = root.get( "strength", 1 ).asFloat() / this->mass * 1000;
	this->color = hex2Color( ( root.isMember( "color" ) ? root[ "color" ] : root[ "colour" ] ).asString() );  // may as well account for English spelling...
	this->isHull = root[ "isHull" ].asBool();
	std::cout << "Adding new Material: \"" << this->name << "\" " << this->color.toString() << "\n";
}
