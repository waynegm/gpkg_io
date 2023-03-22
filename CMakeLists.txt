cmake_minimum_required(VERSION 3.15)
project(
  gpkgio
  VERSION 1.0
  DESCRIPTION "Basic IO library for Geopackage vector data"
  LANGUAGES CXX
)

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
endif()

find_package(SQLite3 REQUIRED)

include (FetchContent)

FetchContent_Declare(
    libgpkg
    GIT_REPOSITORY	https://github.com/luciad/libgpkg.git
    GIT_TAG		9f90a9330d75fd41ff6a857c28f9865c1aa18b7b
)
FetchContent_GetProperties(libgpkg)
if(NOT libgpkg_POPULATED)
  FetchContent_Populate(libgpkg)
  add_subdirectory(${libgpkg_SOURCE_DIR} ${libgpkg_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(${PROJECT_NAME}
    src/utils.h
    src/utils.cc
    src/gpkgio.cc
)
add_library(gpkgio::gpkgio ALIAS gpkgio)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
	${libgpkg_SOURCE_DIR}/gpkg

)

target_compile_features(${PROJECT_NAME}
    PUBLIC
	cxx_std_11
	cxx_variadic_templates
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
	gpkg_ext
    PRIVATE
	SQLite::SQLite3
	gpkg_static
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS gpkgio gpkg_ext)
