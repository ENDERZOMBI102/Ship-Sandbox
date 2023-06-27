#include <fstream>
#include <iostream>
#include <filesystem>

#include "util.hpp"


auto jsonParseFile( const std::string& filename ) -> Json::Value {
	auto contents = readFile( filename );
	if (! contents ) {
		std::cout << "Error: could not open file " << filename << "\n";
		return Json::Value{ };
	}

	Json::Reader reader;
	Json::Value root;
	if ( reader.parse( contents.value().data(), root ) )
		return root;

	std::cerr << "In file " << filename << ": Parsing error(s):\n" << reader.getFormattedErrorMessages();
	return Json::Value{ };
}

auto readFile( const std::string& filename ) -> std::optional<std::span<char>> {
	std::fstream file{ filename.c_str(), std::ios::in | std::ios::binary };

	if (! file.is_open() ) {
		std::cerr << "Could not open file " << filename << " for reading.\n";
		return std::nullopt;
	}

	auto length = std::filesystem::file_size( filename );
	auto buffer = new char[ length ];
	file.read( buffer, static_cast<std::streamsize>( length ) );

	return std::make_optional<std::span<char>>( buffer, length );
}
