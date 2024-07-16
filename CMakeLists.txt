cmake_minimum_required(VERSION 3.28)
project(svitlo VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Function to fetch content if not found
function(fetch_content name url)
    find_package(${name} QUIET)
    if(NOT ${name}_FOUND)
        FetchContent_Declare(${name} URL ${url})
        FetchContent_MakeAvailable(${name})
    endif()
endfunction()

# Fetch or find necessary libraries
fetch_content(CURL https://github.com/curl/curl/archive/refs/tags/curl-7_79_1.tar.gz)
fetch_content(SQLite3 https://sqlite.org/2021/sqlite-autoconf-3350500.tar.gz)
fetch_content(fmt https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz)
fetch_content(raylib https://github.com/raysan5/raylib/archive/master.tar.gz)
fetch_content(nlohmann_json https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.tar.gz)

# Prevent nlohmann_json from building tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# Collect source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

# Exclude specific directories from sources
list(FILTER SOURCES EXCLUDE REGEX ".*(test|example|third_party|deps/nlohmann_json-src/third_party)/.*")

# Define executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set include directories and link libraries
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
    ${fmt_INCLUDE_DIRS}
    ${raylib_SOURCE_DIR}/src
    ${nlohmann_json_SOURCE_DIR}/single_include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
    SQLite::SQLite3
    fmt::fmt
    raylib
    nlohmann_json::nlohmann_json
)

# Install rules
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Copy resource files to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
