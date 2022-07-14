cmake_minimum_required(VERSION 3.0.0)
project(spiff VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

add_executable(spiff src/main.cpp src/proxy.cpp src/packets/packet.cpp)
target_include_directories(spiff PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/3rdparty/asio/asio/include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
