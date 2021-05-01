cmake_minimum_required(VERSION 2.5)

project(conex)

set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC -lphtread")

aux_source_directory(. SRC_LIST)

add_library(conex SHARED ${SRC_LIST})
#add_executable(Echoserver ${SRC_LIST})