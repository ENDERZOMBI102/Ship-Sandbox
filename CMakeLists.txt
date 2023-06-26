cmake_minimum_required( VERSION 3.6 )
project( ShipSandbox LANGUAGES CXX )

set( CMAKE_CXX_STANDARD_REQUIRED 20 )
set( CMAKE_CXX_STANDARD 20 )

add_executable( ShipSandbox
	src/main.cpp
	src/game.cpp
	src/game.hpp
	src/material.cpp
	src/material.hpp
	src/phys.cpp
	src/phys.h
	src/render.cpp
	src/render.hpp
	src/scheduler.cpp
	src/scheduler.h
	src/util.cpp
	src/util.hpp
	src/vec.cpp
	src/vec.hpp
	src/RmlBackend/RmlUi_Platform_GLFW.cpp
	src/RmlBackend/RmlUi_Platform_GLFW.h
	src/RmlBackend/RmlUi_Renderer_GL3.cpp
	src/RmlBackend/RmlUi_Renderer_GL3.h
	src/ShipSandbox.cpp
	src/ShipSandbox.hpp
	src/renderer/debug.hpp
	src/renderer/debug.cpp
	src/renderer/Renderer.hpp
	src/renderer/Renderer.cpp
	src/renderer/buffers.hpp
	src/renderer/buffers.cpp
)
target_compile_definitions( ShipSandbox PRIVATE _CRT_SECURE_NO_WARNINGS )

add_subdirectory( vendor/glad )

if ( UNIX )
	list( APPEND CMAKE_PREFIX_PATH "/home/linuxbrew/.linuxbrew/opt/glfw" )
endif()
find_package( GLFW3 CONFIG REQUIRED )
find_package( jsoncpp CONFIG REQUIRED )
# devil is the actual devil, took me 3h to find this stupid solution...
find_package( PkgConfig REQUIRED )
pkg_check_modules( DevIL IMPORTED_TARGET REQUIRED DevIL )
pkg_check_modules( ILU IMPORTED_TARGET REQUIRED ILU )
find_package( RmlUi CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )

target_link_libraries( ShipSandbox PRIVATE GLAD glfw JsonCpp::JsonCpp PkgConfig::DevIL PkgConfig::ILU RmlCore RmlDebugger spdlog::spdlog spdlog::spdlog_header_only )

if( WIN32 )
	target_link_libraries( ShipSandbox PRIVATE opengl32 )
else()
	target_link_libraries( ShipSandbox PRIVATE m GL )
endif()
