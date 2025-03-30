cmake_minimum_required(VERSION 3.31)
project(ludutils)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(HEADERS
	include/ludutils/lud_archive.hpp
	include/ludutils/lud_id.hpp
	include/ludutils/lud_parse.hpp
	include/ludutils/lud_mem_stream.hpp
	include/ludutils/lud_slurper.hpp
	include/ludutils/lud_assert.hpp
)

add_library(${PROJECT_NAME} INTERFACE)

# this is just to make the files appear on IDEs
target_sources(${PROJECT_NAME} PRIVATE ${HEADERS})


target_include_directories(${PROJECT_NAME} 
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
