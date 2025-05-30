cmake_minimum_required(VERSION 3.31)

set(THIS Reindex)

project(${THIS} CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(
    Gateway
    GIT_REPOSITORY https://github.com/BingTea-SearchEngine/Gateway.git
    GIT_TAG main
)
FetchContent_MakeAvailable(Gateway)

FetchContent_Declare(
    IndexInterface
    GIT_REPOSITORY https://github.com/BingTea-SearchEngine/IndexInterface.git
    GIT_TAG main
)
FetchContent_MakeAvailable(IndexInterface)

find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL Include Dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL Libraries: ${OPENSSL_LIBRARIES}")

set(GATEWAY_SOURCE_DIR ${gateway_SOURCE_DIR})
set(GATEWAY_INCLUDE_DIR "${gateway_SOURCE_DIR}/lib")
message(STATUS "Gateway project source directory: ${GATEWAY_SOURCE_DIR}")
message(STATUS "Gateway include directory: ${GATEWAY_INCLUDE_DIR}")

set(EXPRESSIONPARSER_SOURCE_DIR ${expressionparser_SOURCE_DIR})
set(EXPRESSIONPARSER_INCLUDE_DIR "${expressionparser_SOURCE_DIR}/lib")

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_library(BloomFilter INTERFACE)
target_include_directories(BloomFilter INTERFACE ${LIB_DIR}/BloomFilter ${OPENSSL_INCLUDE_DIR})
if(TARGET OpenSSL::Crypto)
    target_link_libraries(BloomFilter INTERFACE OpenSSL::Crypto)
else()
    message(FATAL_ERROR "OpenSSL::Crypto was not found!")
endif()

add_library(IndexElements STATIC 
    ${LIB_DIR}/IndexElements/MasterChunk.cpp
    ${LIB_DIR}/IndexElements/IndexChunk.cpp
    ${LIB_DIR}/IndexElements/MetadataChunk.cpp
    ${LIB_DIR}/IndexElements/PostingList.cpp
    ${LIB_DIR}/IndexElements/Post.cpp
    ${LIB_DIR}/IndexElements/PostEntry.cpp
    ${LIB_DIR}/IndexElements/WordLocation.cpp
    ${LIB_DIR}/IndexElements/Util.cpp
)
target_include_directories(IndexElements PUBLIC ${LIB_DIR}/IndexElements)
target_link_libraries(IndexElements PUBLIC spdlog::spdlog)

add_library(DocStream STATIC ${LIB_DIR}/DocStream/DocStream.cpp)
target_include_directories(DocStream PUBLIC ${LIB_DIR}/DocStream)
target_link_libraries(DocStream PUBLIC IndexElements BloomFilter)


add_library(IndexStreamReaders STATIC 
    ${LIB_DIR}/IndexStreamReaders/ISRAnd.cpp
    ${LIB_DIR}/IndexStreamReaders/ISROr.cpp
    ${LIB_DIR}/IndexStreamReaders/ISRPhrase.cpp
    ${LIB_DIR}/IndexStreamReaders/ISRWord.cpp
    ${LIB_DIR}/IndexStreamReaders/ISRContainer.cpp
)
target_include_directories(IndexStreamReaders PUBLIC ${LIB_DIR}/IndexStreamReaders)
target_link_libraries(IndexStreamReaders PUBLIC IndexElements)

add_library(QueryCompiler STATIC
  ${LIB_DIR}/QueryCompiler/Expression.cpp
  ${LIB_DIR}/QueryCompiler/Parser.cpp
  ${LIB_DIR}/QueryCompiler/Tokenizer.cpp
)
target_include_directories(QueryCompiler PUBLIC ${LIB_DIR}/QueryCompiler)
target_link_libraries(QueryCompiler PUBLIC IndexStreamReaders IndexElements)

add_executable(${THIS} src/reindex.cpp)
target_link_libraries(${THIS} PUBLIC spdlog::spdlog argparse DocStream IndexElements)
target_include_directories(${THIS} PRIVATE ${LIB_DIR}/DocStream ${LIB_DIR}/IndexElements)

add_executable(IndexServer src/IndexServer.cpp)
target_link_libraries(IndexServer PUBLIC spdlog::spdlog argparse IndexElements GatewayServer
    IndexInterface QueryCompiler)
target_include_directories(IndexServer PUBLIC ${LIB_DIR}/IndexElements ${GATEWAY_INCLUDE_DIR} ${EXPRESSIONPARSER_INCLUDE_DIR})

add_executable(TestExample tests/TestExample.cpp)
target_link_libraries(TestExample PRIVATE GTest::gtest_main)

add_executable(TestMasterChunk tests/TestMasterChunk.cpp)
target_link_libraries(TestMasterChunk PRIVATE GTest::gtest_main IndexElements)

add_executable(TestPostingList tests/TestPostingList.cpp)
target_link_libraries(TestPostingList PRIVATE GTest::gtest_main IndexElements)

add_executable(TestIndexChunk tests/TestIndexChunk.cpp)
target_link_libraries(TestIndexChunk PRIVATE GTest::gtest_main IndexElements)

add_executable(TestMetadataChunk tests/TestMetadataChunk.cpp)
target_link_libraries(TestMetadataChunk PRIVATE GTest::gtest_main IndexElements)

add_executable(TestISRWord tests/TestISRWord.cpp)
target_link_libraries(TestISRWord PRIVATE GTest::gtest_main IndexStreamReaders)

add_executable(TestISROr tests/TestISROr.cpp)
target_link_libraries(TestISROr PRIVATE GTest::gtest_main IndexStreamReaders)

add_executable(TestISRAnd tests/TestISRAnd.cpp)
target_link_libraries(TestISRAnd PRIVATE GTest::gtest_main IndexStreamReaders)

add_executable(TestISRPhrase tests/TestISRPhrase.cpp)
target_link_libraries(TestISRPhrase PRIVATE GTest::gtest_main IndexStreamReaders)

add_executable(TestISRContainer tests/TestISRContainer.cpp)
target_link_libraries(TestISRContainer PRIVATE GTest::gtest_main IndexStreamReaders)

add_executable(TestQueryCompiler tests/TestQueryCompiler.cpp)
target_link_libraries(TestQueryCompiler PRIVATE GTest::gtest_main QueryCompiler)

add_executable(TestISRSynchronizationTable tests/TestISRSynchronizationTable.cpp)
target_link_libraries(TestISRSynchronizationTable PRIVATE GTest::gtest_main IndexStreamReaders)

include(GoogleTest)
gtest_discover_tests(TestExample)
gtest_discover_tests(TestMasterChunk)
gtest_discover_tests(TestPostingList)
gtest_discover_tests(TestIndexChunk)
gtest_discover_tests(TestMetadataChunk)

gtest_discover_tests(TestISRWord)
gtest_discover_tests(TestISROr)
gtest_discover_tests(TestISRAnd)
gtest_discover_tests(TestISRPhrase)
gtest_discover_tests(TestISRContainer)
gtest_discover_tests(TestQueryCompiler)
gtest_discover_tests(TestISRSynchronizationTable)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
