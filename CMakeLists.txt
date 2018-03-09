cmake_minimum_required (VERSION 2.8)
################################################################################
set(CMAKE_BUILD_TYPE Debug)
set(GLOB BOOST_VERSION 1.55)

################################################################################
add_subdirectory(./i2pd/build)

################################################################################
project(i2poui)
find_package(Boost ${BOOST_VERSION} COMPONENTS filesystem
                                               program_options
                                               thread
                                               system
                                               coroutine
                                               REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -ggdb2 ${SANITIZE} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -ggdb ${SANITIZE} ")

include_directories(
    "${Boost_INCLUDE_DIR}"
    "./include/"
    "./i2pd/libi2pd"
    "./i2pd/libi2pd_client")

set(sources
    "./src/service.cpp"
    "./src/server.cpp"
    "./src/client.cpp"
    "./src/connection.cpp")

add_library(i2poui ${sources})
add_dependencies(i2poui i2pd)

target_link_libraries(i2poui
    i2pdclient
    libi2pd
    ssl
    crypto
    z
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})

################################################################################
set(I2POUI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)

################################################################################
project(chat)

find_package(Boost ${BOOST_VERSION} COMPONENTS filesystem
                                               program_options
                                               thread
                                               system
                                               coroutine
                                               REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -ggdb2 ${SANITIZE} -O0")

include_directories(
    "${Boost_INCLUDE_DIR}"
    "./include/")

set(sources "./example/chat.cpp")

add_executable(chat ${sources})
add_dependencies(chat i2poui)

target_link_libraries(chat
    i2poui
    i2pdclient
    libi2pd
    ssl
    crypto
    z
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})

