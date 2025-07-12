cmake_minimum_required(VERSION 3.8)
project(jnp5)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        keyed_queue.h
        keyed_queueX.h
        throwers.h
        )

add_executable(example example.cpp ${SOURCE_FILES})
add_executable(miniTest miniTest.cpp ${SOURCE_FILES})
add_executable(exception_handling exception_handling.cc ${SOURCE_FILES})