cmake_minimum_required(VERSION 3.28)
project(svitlo VERSION 1.0 LANGUAGES CXX)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# function to fetch content if not found
function(fetch_and_include_library name url)
    find_package(${name} QUIET)
    if(NOT ${name}_FOUND)
        FetchContent_Declare(
            ${name}
            URL ${url}
        )
        FetchContent_MakeAvailable(${name})
    endif()
endfunction()

# fetch or find necessary libraries
fetch_and_include_library(CURL https://github.com/curl/curl/archive/refs/tags/curl-7_79_1.tar.gz)
fetch_and_include_library(SQLite3 https://sqlite.org/2021/sqlite-autoconf-3350500.tar.gz)
fetch_and_include_library(fmt https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz)
fetch_and_include_library(raylib https://github.com/raysan5/raylib/archive/master.tar.gz)

# collect source files
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

# define executable
Add_executable(${PROJECT_NAME} ${SOURCES})

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
    SQLite::SQLite3
    fmt::fmt
    raylib
)

# install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# copy resource files to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
