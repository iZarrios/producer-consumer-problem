cmake_minimum_required(VERSION 3.20)

project(producer_consumer)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)

add_executable(producer ${EXAMPLES_DIR}/producer.cpp)
target_link_libraries(producer lib1)

add_executable(consumer ${EXAMPLES_DIR}/consumer.cpp)
target_link_libraries(consumer lib1)
