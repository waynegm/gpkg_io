cmake_minimum_required(VERSION 3.15)
project(
  gpkgio
  VERSION 1.0
  DESCRIPTION "Basic IO library for Geopackage vector data"
  LANGUAGES CXX
)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

find_package(SQLite3 REQUIRED)

add_subdirectory(external EXCLUDE_FROM_ALL)

add_subdirectory( src )

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    find_package(Doxygen)
    if(Doxygen_FOUND)
	add_subdirectory(docs)
    else()
	message(STATUS "Doxygen not found, not building docs")
    endif()

    include(CTest)
    add_subdirectory(tests)

endif()

install(TARGETS  gpkgio DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(TARGETS  gpkg_ext DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
