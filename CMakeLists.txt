cmake_minimum_required( VERSION 3.6 )
project( ShipSandbox LANGUAGES CXX )

set( CMAKE_CXX_STANDARD_REQUIRED 20 )
set( CMAKE_CXX_STANDARD 20 )

add_executable( ShipSandbox
	src/main.cpp
	src/game.cpp
	src/game.h
	src/material.cpp
	src/material.h
	src/phys.cpp
	src/phys.h
	src/render.cpp
	src/render.h
	src/scheduler.cpp
	src/scheduler.h
	src/util.cpp
	src/util.h
	src/vec.cpp
	src/vec.h
)
target_compile_definitions( ShipSandbox PRIVATE "$<$<CONFIG:DEBUG>:__WXDEBUG__>" __WXMSW__ WXUSINGDLL wxUSE_UNICODE _CRT_SECURE_NO_WARNINGS )

add_subdirectory( vendor/glad )

#find_package( OpenGL REQUIRED )
find_package( GLFW3 CONFIG REQUIRED )
find_package( jsoncpp CONFIG REQUIRED )
# devil is the actual devil, took me 3h to find this stupid solution...
find_package( PkgConfig REQUIRED )
pkg_check_modules( DevIL IMPORTED_TARGET REQUIRED DevIL )
pkg_check_modules( ILU IMPORTED_TARGET REQUIRED ILU )

target_link_libraries( ShipSandbox PRIVATE GLAD glfw JsonCpp::JsonCpp PkgConfig::DevIL PkgConfig::ILU )

if( WIN32 )
	target_link_libraries( ShipSandbox PRIVATE opengl32 )
else()
	target_link_libraries( ShipSandbox PRIVATE m GL )
endif()

