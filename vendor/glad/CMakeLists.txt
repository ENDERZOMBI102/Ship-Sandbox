cmake_minimum_required( VERSION 3.24.0 )
project( GLAD LANGUAGES C )

add_library( GLAD src/glad.c include/glad/glad.h include/KHR/khrplatform.h )
set_target_properties( GLAD PROPERTIES LINKER_LANGUAGE C )

target_include_directories( GLAD PUBLIC include)