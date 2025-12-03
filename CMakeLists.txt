cmake_minimum_required(VERSION 3.31)

project(ludutils)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(LUDUTILS_TESTS "Enable tests project" OFF)

set(HEADERS
	include/ludutils/lud_id.hpp
	include/ludutils/lud_parse.hpp
	include/ludutils/lud_mem_stream.hpp
	include/ludutils/lud_assert.hpp
	include/ludutils/lud_misc.hpp
)


add_library(${PROJECT_NAME} INTERFACE)

# this is just to make the files appear on IDEs
target_sources(${PROJECT_NAME} PRIVATE ${HEADERS})

target_compile_definitions(${PROJECT_NAME} INTERFACE CMAKE_EXPORT_COMPILE_COMMANDS=1)


target_include_directories(${PROJECT_NAME} 
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)



if (LUDUTILS_TESTS)
	include(FetchContent)

	FetchContent_Declare(
		Catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG        v3.11.0
		EXCLUDE_FROM_ALL
	)

	FetchContent_MakeAvailable(Catch2)


	set(ludutil_test_dir ${CMAKE_CURRENT_LIST_DIR}/tests)

	add_executable(LUDUTILS_TESTS)
	target_sources(LUDUTILS_TESTS PRIVATE
		${ludutil_test_dir}/test_parse.cpp
	)

	target_link_libraries(LUDUTILS_TESTS
		PRIVATE ludutils
		PRIVATE Catch2::Catch2WithMain
	)


	
	list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
	include(CTest)
	include(Catch)
	catch_discover_tests(LUDUTILS_TESTS)

endif()
