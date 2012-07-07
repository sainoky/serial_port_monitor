set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.6)

project(fancy_cui)

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_C_FLAGS "-O2 -Wall -Wextra")
elseif (MSVC)
	set(CMAKE_C_FLAGS					"/W4")
	set(CMAKE_C_FLAGS_DEBUG				"/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
	set(CMAKE_C_FLAGS_RELEASE			"/MT /O2 /Ob2 /D NDEBUG")
endif()

aux_source_directory(src SRC)
add_executable(fancy_cui ${SRC})
