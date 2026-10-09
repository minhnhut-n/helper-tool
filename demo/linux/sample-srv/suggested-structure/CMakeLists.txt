cmake_minimum_required(VERSION 3.16)
project(sample-srv VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(sample-srv
	src/main.c
	src/config.c
	src/logger.c
	src/signal_handler.c
	src/worker.c
	src/service.c
)

target_include_directories(sample-srv PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
	target_compile_options(sample-srv PRIVATE /W4)
else()
	target_compile_options(sample-srv PRIVATE -Wall -Wextra)
endif()

