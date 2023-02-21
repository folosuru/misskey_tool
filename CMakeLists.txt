cmake_minimum_required(VERSION 3.23)
project(misskey_tool)

set(CMAKE_CXX_STANDARD 14)

find_package(cpprestsdk REQUIRED)
find_package(libpqxx REQUIRED)

file(GLOB SOURCES "/*.cpp")

add_executable(misskey_tool main.cpp  ${SOURCES} ${LIBREST})

target_link_libraries(misskey_tool PRIVATE cpprestsdk::cpprest)

target_link_libraries(misskey_tool PRIVATE libpqxx::pqxx)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")