add_executable( Renderer2
	src/main.cpp
	src/renderer/buffers.cpp
	src/renderer/buffers.hpp
	src/renderer/debug.cpp
	src/renderer/debug.hpp
	src/renderer/Renderer.cpp
	src/renderer/Renderer.hpp
	src/util/stringable.cpp
	src/util/stringable.hpp
)
target_compile_definitions( Renderer2 PRIVATE _CRT_SECURE_NO_WARNINGS GLFW_INCLUDE_NONE )

if ( UNIX )
	list( APPEND CMAKE_PREFIX_PATH "/home/linuxbrew/.linuxbrew/opt/glfw" )
endif()
find_package( GLFW3 CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )
find_package( glm CONFIG REQUIRED )

target_link_libraries( Renderer2 PRIVATE GLAD glfw spdlog::spdlog spdlog::spdlog_header_only glm::glm )

if( WIN32 )
	target_link_libraries( Renderer2 PRIVATE opengl32 )
else()
	target_link_libraries( Renderer2 PRIVATE m GL )
endif()
