cmake_minimum_required(VERSION 3.28)

project(svitlo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# function to fetch content if not found
function(fetch_content_if_not_found name url)
    find_package(${name} QUIET)
    if(NOT ${name}_FOUND)
        FetchContent_Declare(
            ${name}
            URL ${url}
            FIND_PACKAGE_ARGS
        )
        FetchContent_MakeAvailable(${name})
    endif()
endfunction()

# fetch or find necessary libraries
fetch_content_if_not_found(CURL https://github.com/curl/curl/archive/refs/tags/curl-7_79_1.tar.gz)
fetch_content_if_not_found(SQLite3 https://sqlite.org/2021/sqlite-autoconf-3350500.tar.gz)
fetch_content_if_not_found(fmt https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz)
fetch_content_if_not_found(raylib https://github.com/raysan5/raylib/archive/master.tar.gz)
fetch_content_if_not_found(nlohmann_json https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.tar.gz)
fetch_content_if_not_found(raygui https://github.com/raysan5/raygui/archive/master.tar.gz)

# handle raygui
FetchContent_GetProperties(raygui)
if(raygui_POPULATED)
    set(RAYGUI_INCLUDE_DIR ${raygui_SOURCE_DIR}/src)
    file(COPY ${RAYGUI_INCLUDE_DIR}/raygui.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include)
   
    set(STYLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res/styles)
    file(MAKE_DIRECTORY ${STYLES_DIR})
    file(COPY ${raygui_SOURCE_DIR}/styles/ashes DESTINATION ${STYLES_DIR})
endif()

# prevent nlohmann_json from building tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# collect source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

# exclude specific directories from sources
list(FILTER SOURCES EXCLUDE REGEX ".*(test|example|third_party|deps/nlohmann_json-src/third_party)/.*")

# define executable
add_executable(${PROJECT_NAME} ${SOURCES})

# set include directories and link libraries
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${RAYGUI_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
    SQLite::SQLite3
    fmt::fmt
    raylib
    nlohmann_json::nlohmann_json
)

# install rules
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# copy resource files to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)
