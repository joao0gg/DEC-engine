cmake_minimum_required(VERSION 3.14)
project(DEC-engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel)
endif()

include(FetchContent)

FetchContent_Declare(geometry-central
  GIT_REPOSITORY https://github.com/nmwsharp/geometry-central.git
  GIT_TAG        019669ddabda05e0f71fa3587cfb3c1dadf19cb8
)
FetchContent_Declare(polyscope
  GIT_REPOSITORY https://github.com/nmwsharp/polyscope.git
  GIT_TAG        59da72df6517cab8379865899bdffdbc96171301
)
FetchContent_MakeAvailable(geometry-central polyscope)

file(GLOB_RECURSE DEC_ENGINE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(dec_engine ${DEC_ENGINE_SOURCES})
target_include_directories(dec_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(dec_engine PUBLIC geometry-central)

add_executable(dec_example apps/example/main.cpp)
target_link_libraries(dec_example PRIVATE dec_engine polyscope)