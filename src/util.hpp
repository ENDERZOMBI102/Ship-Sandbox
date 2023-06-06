#pragma once

#include <optional>
#include <span>
#include <string>
#include "json/json.h"

auto jsonParseFile( const std::string& filename ) -> Json::Value;
auto readFile( const std::string& filename ) -> std::optional<std::span<char>>;
