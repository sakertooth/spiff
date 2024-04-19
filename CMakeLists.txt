cmake_minimum_required(VERSION 3.0.0)
project(spiff VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

# Create spiff
add_executable(spiff src/main.cpp src/proxy.cpp src/varint_varlong.cpp)
target_include_directories(spiff PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/3rdparty/asio/asio/include)

if(WIN32)
  target_link_libraries(spiff wsock32 ws2_32)
endif()

# Add tests
add_subdirectory(tests)
add_test(NAME VarintVarlong COMMAND varint_varlong)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
