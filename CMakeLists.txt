cmake_minimum_required(VERSION 3.14...3.22)

# ---- Project ----
project(
  Svitlo
  VERSION 1.2
  LANGUAGES CXX
)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add dependencies via FetchContent ----
include(FetchContent)

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

fetch_content_if_not_found(fmt https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz)
fetch_content_if_not_found(CURL https://github.com/curl/curl/archive/refs/tags/curl-7_79_1.tar.gz)
fetch_content_if_not_found(SQLite3 https://sqlite.org/2021/sqlite-autoconf-3350500.tar.gz)

# ---- Add source files ----
set(headers
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/electricity-data.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/consumer.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/database-manager.h" 
)
set(sources
    "${CMAKE_CURRENT_SOURCE_DIR}/source/electricity-data.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/consumer.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/database-manager.cpp" 
)

# ---- Create library ----
add_library(${PROJECT_NAME} ${headers} ${sources})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

target_include_directories(${PROJECT_NAME} 
  PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} 
  PUBLIC 
    fmt::fmt 
    CURL::libcurl 
    SQLite::SQLite3
)

# ---- Install rules ----
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
