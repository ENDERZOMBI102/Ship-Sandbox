#include <iostream>
#include <string>
#include <RmlUi/Core/Log.h>

#include "material.hpp"


vec3f hex2Color( std::string str ) {  //  e.g. "#00FF00";
	if ( str[ 0 ] == '#' )
		str = str.substr( 1 );
	if ( str.length() != 6 )
		std::cout << "Error: badly formed hex color value!\n";

	auto color = std::stoi( str, nullptr, 16 );

	return vec3f{
		static_cast<float>( ( color & 0xFF'00'00 ) >> 16 ) / 255,  // R
		static_cast<float>( ( color & 0x00'FF'00 ) >>  8 ) / 255,  // G
		static_cast<float>( ( color & 0x00'00'FF ) >>  0 ) / 255   // B
	};
}

Material::Material( const Json::Value& root ) {
	this->name = root.get( "name", "Unspecified" ).asString();
	this->mass = root.get( "mass", 1 ).asFloat();
	this->strength = root.get( "strength", 1 ).asFloat() / this->mass * 1000;
	this->color = hex2Color( ( root.isMember( "color" ) ? root[ "color" ] : root[ "colour" ] ).asString() );  // may as well account for English spelling...
	this->isHull = root[ "isHull" ].asBool();
	Rml::Log::Message( Rml::Log::Type::LT_INFO, "Adding new Material: \"%s\" %s", this->name.c_str(), this->color.toString().c_str() );
}
