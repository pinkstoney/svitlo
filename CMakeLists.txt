cmake_minimum_required(VERSION 3.28)
project(svitlo)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)  # Add this line

# Replace this path with the path to your Raylib installation
list(APPEND CMAKE_PREFIX_PATH "/path/to/your/raylib")

find_package(raylib 3.0 QUIET) # Let's find the installed raylib

if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            URL https://github.com/raysan5/raylib/archive/master.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

add_executable(svitlo main.cpp
        src/shutdown-info.cpp
        include/shutdown-info.h
        include/circle.h
        src/circle.cpp
        src/shutdown-circle.cpp
        include/shutdown-circle.h
        include/application.h
        src/application.cpp
        libs/raygui.cpp
        src/database-manager.cpp
        include/database-manager.h
)

include_directories(${CMAKE_SOURCE_DIR}/libs)
target_link_libraries(svitlo ${CURL_LIBRARIES} raylib SQLite::SQLite3)  # Add SQLite3 here