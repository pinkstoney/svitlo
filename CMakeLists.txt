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
fetch_content_if_not_found(nlohmann_json https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.tar.gz)

# ---- Add source files ----
set(headers
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/electricity-data.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/consumer.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/database-manager.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/data-fetcher.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/svitlo/json-parser.h"
)
set(sources
    "${CMAKE_CURRENT_SOURCE_DIR}/source/electricity-data.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/consumer.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/database-manager.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/data-fetcher.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/json-parser.cpp" 
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

# Link libraries, handling cases where targets might not exist
if(TARGET fmt::fmt)
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
elseif(TARGET fmt)
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt)
endif()

if(TARGET CURL::libcurl)
    target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)
elseif(TARGET CURL::CURL)
    target_link_libraries(${PROJECT_NAME} PUBLIC CURL::CURL)
elseif(TARGET CURL)
    target_link_libraries(${PROJECT_NAME} PUBLIC CURL)
endif()

if(TARGET SQLite::SQLite3)
    target_link_libraries(${PROJECT_NAME} PUBLIC SQLite::SQLite3)
elseif(TARGET sqlite3)
    target_link_libraries(${PROJECT_NAME} PUBLIC sqlite3)
endif()

if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
elseif(TARGET nlohmann_json)
    target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json)
endif()

# ---- Install rules ----
include(GNUInstallDirs)

# Function to safely add targets to the export set
function(safe_install_dependency dep)
    if(TARGET ${dep})
        get_target_property(target_type ${dep} TYPE)
        if(target_type STREQUAL "INTERFACE_LIBRARY")
            install(TARGETS ${dep} EXPORT ${PROJECT_NAME}Targets)
        elseif(NOT target_type STREQUAL "ALIAS_TARGET")
            install(TARGETS ${dep}
                EXPORT ${PROJECT_NAME}Targets
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )
        endif()
    endif()
endfunction()

# Safely install dependencies
safe_install_dependency(fmt)
safe_install_dependency(CURL)
safe_install_dependency(sqlite3)
safe_install_dependency(nlohmann_json)

# Install the main project target
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

# Generate and install package configuration files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
