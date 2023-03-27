cmake_minimum_required(VERSION 3.23)
project(misskey_tool)
project(updater)

set(CMAKE_CXX_STANDARD 17)

find_package(cpprestsdk REQUIRED)
find_package(libpqxx REQUIRED)
find_package(nlohmann_json REQUIRED)

find_library(HIREDIS_LIB hiredis)
find_library(REDIS_PLUS_PLUS_LIB redis++)

file(GLOB_RECURSE api
    ${PROJECT_SOURCE_DIR}/src/software_api/*.cpp
    ${PROJECT_SOURCE_DIR}/src/software_api/*.hpp
)
file(GLOB_RECURSE util
        ${PROJECT_SOURCE_DIR}/src/util/*.cpp
        ${PROJECT_SOURCE_DIR}/src/util/*.hpp
)

add_executable(misskey_tool src/crawler.cpp ${api} ${util})
target_link_libraries(misskey_tool cpprestsdk::cpprest)
target_link_libraries(misskey_tool libpqxx::pqxx)
target_link_libraries(misskey_tool nlohmann_json::nlohmann_json)

add_executable(updater src/updater.cpp ${api} ${util})
target_link_libraries(updater cpprestsdk::cpprest)
target_link_libraries(updater libpqxx::pqxx)
target_link_libraries(updater nlohmann_json::nlohmann_json)

target_link_libraries(misskey_tool ${HIREDIS_LIB})
target_link_libraries(misskey_tool ${REDIS_PLUS_PLUS_LIB})

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")