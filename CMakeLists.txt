cmake_minimum_required(VERSION 3.31)

set(THIS Reindex)

project(${THIS} CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_library(IndexElements STATIC 
    ${LIB_DIR}/IndexElements/MasterChunk.cpp
    ${LIB_DIR}/IndexElements/IndexChunk.cpp
    ${LIB_DIR}/IndexElements/PostingList.cpp
    ${LIB_DIR}/IndexElements/Post.cpp
    ${LIB_DIR}/IndexElements/Types.hpp
)
target_include_directories(IndexElements PUBLIC ${LIB_DIR}/IndexElements)

add_library(DocStream STATIC ${LIB_DIR}/DocStream/DocStream.cpp)
target_include_directories(DocStream PUBLIC ${LIB_DIR}/DocStream)
target_link_libraries(DocStream PUBLIC IndexElements)


add_executable(${THIS} src/reindex.cpp)
target_link_libraries(${THIS} PUBLIC spdlog::spdlog argparse DocStream IndexElements)
target_include_directories(${THIS} PRIVATE ${LIB_DIR}/DocStream ${LIB_DIR}/IndexElements)

add_executable(TestExample tests/TestExample.cpp)
target_link_libraries(TestExample PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(TestExample)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
