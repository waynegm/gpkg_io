cmake_minimum_required(VERSION 3.15)
project(
  gpkg_io
  VERSION 1.0
  LANGUAGES CXX
)
include(CTest)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

include(FetchContent)
#
# Get the sqlite amalgamation and build a static library
#
FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2022/sqlite-amalgamation-3380200.zip
)
message(STATUS "Fetching SQLite...")
FetchContent_GetProperties(sqlite3)
if (NOT sqlite3_POPULATED)
    FetchContent_Populate(sqlite3)
    configure_file(sqlite3_CMakeLists.in ${sqlite3_SOURCE_DIR}/CMakeLists.txt COPYONLY)
    add_subdirectory(${sqlite3_SOURCE_DIR} ${sqlite3_BINARY_DIR})
endif()

set(SQLITE3_INCLUDE_DIR ${sqlite3_SOURCE_DIR} CACHE FILEPATH "x" FORCE)

message(STATUS "Building SQLite...")
execute_process(
    COMMAND ${CMAKE_COMMAND} ${sqlite3_SOURCE_DIR} -B ${sqlite3_BINARY_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${sqlite3_BINARY_DIR}
)
find_library(
    SQLITE3_LIBRARY
    NAMES sqlite3
    PATHS ${sqlite3_BINARY_DIR}
    NO_DEFAULT_PATH
)

add_subdirectory( gpkg_io_src )

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

if (BUILD_TESTING)
    add_subdirectory( test )
endif()

