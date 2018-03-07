cmake_minimum_required (VERSION 2.8)
################################################################################
set(CMAKE_BUILD_TYPE Debug)
set(BOOST_VERSION 1.62)

################################################################################
option(WITH_BINARY "" OFF)
option(WITH_GUI "" OFF)
add_subdirectory(./i2pd/build)

################################################################################
project(i2poui)
find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -ggdb ${SANITIZE}")

include_directories(
    "${Boost_INCLUDE_DIR}"
    "./include/"
    "./i2pd/libi2pd"
    "./i2pd/libi2pd_client")

set(sources
    "./src/channel.cpp"
    "./src/service.cpp"
    "./src/acceptor.cpp")

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
if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Android")

project(chat)

find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS filesystem
                                                              program_options
                                                              date_time
                                                              thread
                                                              system
                                                              coroutine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -ggdb ${SANITIZE}")

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

endif() # if not Android

