cmake_minimum_required( VERSION 3.6 )
project( ShipSandbox LANGUAGES CXX )

add_executable( ShipSandbox
	main.cpp
	game.cpp
	game.h
	material.cpp
	material.h
	phys.cpp
	phys.h
	render.cpp
	render.h
	scheduler.cpp
	scheduler.h
	util.cpp
	util.h
	vec.cpp
	vec.h
)
target_compile_definitions( ShipSandbox PRIVATE "$<$<CONFIG:DEBUG>:__WXDEBUG__>" __WXMSW__ WXUSINGDLL wxUSE_UNICODE _CRT_SECURE_NO_WARNINGS )

add_subdirectory( vendor/glad )

#find_package( OpenGL REQUIRED )
find_package( GLFW3 CONFIG REQUIRED )
find_package( jsoncpp CONFIG REQUIRED )
find_package( devil CONFIG REQUIRED )
target_link_libraries( ShipSandbox PRIVATE m vendor/glad glfw JsonCpp::JsonCpp )

if( WIN32 )
	target_link_libraries( ShipSandbox PRIVATE opengl32 )
else()
	target_link_libraries( ShipSandbox PRIVATE GL )
endif()

