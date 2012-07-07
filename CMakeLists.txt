set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.6)

project(fancy_cui)

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_C_FLAGS "-O2 -Wall -Wextra")
endif()

aux_source_directory(src SRC)
add_executable(fancy_cui ${SRC})
