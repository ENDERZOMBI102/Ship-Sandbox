add_library( Renderer2 STATIC
	src/renderer/buffers.cpp
	src/renderer/buffers.hpp
	src/renderer/debug.cpp
	src/renderer/debug.hpp
	src/renderer/Renderer.cpp
	src/renderer/Renderer.hpp
	src/util/stringable.hpp
	src/renderer/Shader.cpp
	src/renderer/Shader.hpp
)
target_compile_definitions( Renderer2 PUBLIC _CRT_SECURE_NO_WARNINGS GLFW_INCLUDE_NONE )

find_package( GLFW3 CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )
find_package( glm CONFIG REQUIRED )

target_link_libraries( Renderer2 PUBLIC GLAD glfw spdlog::spdlog spdlog::spdlog_header_only glm::glm )

if( WIN32 )
	target_link_libraries( Renderer2 PRIVATE opengl32 )
else()
	target_link_libraries( Renderer2 PRIVATE m GL )
endif()

add_executable( Renderer2_test test/main.cpp )
target_link_libraries( Renderer2_test PRIVATE Renderer2 )
