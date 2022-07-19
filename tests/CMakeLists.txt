add_executable(varint_varlong varint_varlong_test.cpp ${CMAKE_SOURCE_DIR}/src/varint_varlong.cpp)
target_include_directories(varint_varlong PRIVATE ${CMAKE_SOURCE_DIR}/include)