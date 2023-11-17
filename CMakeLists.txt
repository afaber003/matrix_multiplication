cmake_minimum_required(VERSION 3.23)
project(multiThreadMatrix)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(multiThreadMatrix main.cpp)
target_link_libraries(multiThreadMatrix PRIVATE Threads::Threads)
