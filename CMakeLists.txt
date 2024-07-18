cmake_minimum_required(VERSION 3.28)
project(svitlo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(ExternalProject)

function(fetch_content_if_not_found name url)
    find_package(${name} QUIET)
    if(NOT ${name}_FOUND)
        FetchContent_Declare(
            ${name}
            URL ${url}
        )
        FetchContent_MakeAvailable(${name})
    endif()
endfunction()

# Fetch dependencies
fetch_content_if_not_found(CURL https://github.com/curl/curl/archive/refs/tags/curl-7_79_1.tar.gz)
fetch_content_if_not_found(SQLite3 https://sqlite.org/2021/sqlite-autoconf-3350500.tar.gz)
fetch_content_if_not_found(fmt https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz)
fetch_content_if_not_found(raylib https://github.com/raysan5/raylib/archive/master.tar.gz)
fetch_content_if_not_found(nlohmann_json https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.tar.gz)
fetch_content_if_not_found(raygui https://github.com/raysan5/raygui/archive/master.tar.gz)

# Configure raygui
FetchContent_GetProperties(raygui)
if(raygui_POPULATED)
    set(RAYGUI_INCLUDE_DIR ${raygui_SOURCE_DIR}/src)
    file(COPY ${RAYGUI_INCLUDE_DIR}/raygui.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set(STYLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res/styles)
    file(MAKE_DIRECTORY ${STYLES_DIR})
    file(COPY ${raygui_SOURCE_DIR}/styles/dark DESTINATION ${STYLES_DIR})
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")

# Fetch and configure googletest
ExternalProject_Add(
    googletest
    URL https://github.com/google/googletest/archive/release-1.11.0.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/googletest
        -DBUILD_GMOCK=OFF
        -DINSTALL_GTEST=ON
        -Dgtest_force_shared_crt=ON
    BUILD_BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/googletest/lib/libgtest.a
        ${CMAKE_CURRENT_BINARY_DIR}/googletest/lib/libgtest_main.a
)
add_custom_target(googletest_build DEPENDS googletest)

set(GTEST_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/googletest/include)
set(GTEST_LIBRARIES ${CMAKE_BINARY_DIR}/googletest/lib/libgtest.a ${CMAKE_BINARY_DIR}/googletest/lib/libgtest_main.a)

enable_testing()

# Gather library sources
file(GLOB_RECURSE LIB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*(test|example|third_party|deps/nlohmann_json-src/third_party)/.*")

# Create project library
add_library(${PROJECT_NAME}_lib ${LIB_SOURCES})
target_include_directories(${PROJECT_NAME}_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${RAYGUI_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    CURL::libcurl
    SQLite::SQLite3
    fmt::fmt
    raylib
    nlohmann_json::nlohmann_json
)

# Create main executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# Create test executable
add_executable(${PROJECT_NAME}_test 
    tests/test_shutdown_data.cpp
)
add_dependencies(${PROJECT_NAME}_test googletest_build)
target_include_directories(${PROJECT_NAME}_test PRIVATE 
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}_test PRIVATE
    ${GTEST_LIBRARIES}
    ${PROJECT_NAME}_lib
    pthread
)

# Add custom target for building tests
add_custom_target(build_tests ALL DEPENDS ${PROJECT_NAME}_test)

# Add test
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test --gtest_color=yes --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_report.xml)

set(CMAKE_CTEST_ARGUMENTS "--output-on-failure --verbose")

# Install target
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)
