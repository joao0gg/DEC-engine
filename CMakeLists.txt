cmake_minimum_required(VERSION 3.14)
project(DEC-engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(geometry-central
  GIT_REPOSITORY https://github.com/nmwsharp/geometry-central.git
  GIT_TAG master
)
FetchContent_Declare(polyscope
  GIT_REPOSITORY https://github.com/nmwsharp/polyscope.git
  GIT_TAG master
)
FetchContent_MakeAvailable(geometry-central polyscope)

file(GLOB_RECURSE DEC_ENGINE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(dec_engine ${DEC_ENGINE_SOURCES})
target_include_directories(dec_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(dec_engine PUBLIC geometry-central polyscope)

add_executable(dec_example apps/example/main.cpp)
target_link_libraries(dec_example PRIVATE dec_engine)
target_compile_definitions(dec_example PRIVATE DEC_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
