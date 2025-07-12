cmake_minimum_required(VERSION 2.8)
project (sort)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

add_executable(sort th.c alloc.h alloc.c)
target_link_libraries(sort Threads::Threads)
