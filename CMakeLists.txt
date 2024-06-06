cmake_minimum_required(VERSION 3.0)

project(Project)

set(CMAKE_BUILD_TYPE)

set(
  SOURCES 
  
  Source.cpp
  Regex.cpp
  Interpreter.cpp
  ResManager.cpp
  AST.cpp
  Utility.cpp
  Token.cpp
  Error.cpp
)

add_executable(canvas ${SOURCES})

find_package(Boost REQUIRED COMPONENTS regex)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(canvas ${Boost_LIBRARIES})


if(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  target_compile_options(canvas PRIVATE -O2)
elseif(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  target_compile_options(canvas PRIVATE -g)
endif()
