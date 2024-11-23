cmake_minimum_required( VERSION 3.15 )

set( gpkgio_VERSION_MAJOR 1 )
set( gpkgio_VERSION_MINOR 0 )
set( gpkgio_VERSION_PATCH 1 )
set( gpkgio_VERSION ${gpkgio_VERSION_MAJOR}.${gpkgio_VERSION_MINOR}.${gpkgio_VERSION_PATCH} )

if ( POLICY CMP0048 )
    cmake_policy( SET CMP0048 NEW )
endif()

project(
  gpkgioLib
  VERSION ${gpkgio_VERSION}
  DESCRIPTION "Basic IO library for Geopackage vector data"
  LANGUAGES CXX
)

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    find_package(Doxygen)
    if(Doxygen_FOUND)
	add_subdirectory(docs)
    else()
	message(STATUS "Doxygen not found, not building docs")
    endif()

    include(CTest)
endif()

find_package( SQLite3 QUIET )
if ( NOT SQLite3_FOUND )
    find_package( SQLite3 REQUIRED )
endif()

include( FetchContent )

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

get_target_property( gpkg_VERSION gpkg_ext VERSION )

include(GenerateExportHeader)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if( NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()

if( (CMAKE_CONFIGURATION_TYPES OR MSVC) AND NOT CMAKE_DEBUG_POSTFIX )
  set( CMAKE_DEBUG_POSTFIX _d )
endif()

add_library( gpkgio
	include/gpkgio/gpkgio.h
	src/gpkgio.cc
	src/utils.cc
	src/utils.h
)
generate_export_header( gpkgio )
set_target_properties( gpkgio PROPERTIES
	VERSION ${gpkgio_VERSION}
	SOVERSION ${gpkgio_VERSION_MAJOR}
	INTERFACE_gpkgio_MAJOR_VERSION ${gpkgio_VERSION_MAJOR}
	COMPATIBLE_INTERFACE_STRING gpkgio_VERSION_MAJOR )

target_include_directories( gpkgio
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}
	${libgpkg_SOURCE_DIR}/gpkg
)

target_link_libraries( gpkgio
    PUBLIC
	gpkg_ext
    PRIVATE
	gpkg_static
	${gpkg}
	SQLite::SQLite3
)

if ( CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING )
    add_subdirectory(tests)
endif()

install( TARGETS gpkg_ext EXPORT gpkgTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install( TARGETS gpkg_static EXPORT gpkgTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install( TARGETS gpkgio EXPORT gpkgioTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install( FILES include/gpkgio/gpkgio.h "${CMAKE_CURRENT_BINARY_DIR}/gpkgio_export.h"
    DESTINATION include
    COMPONENT Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/gpkg/gpkgConfigVersion.cmake"
  VERSION ${gpkg_VERSION}
  COMPATIBILITY AnyNewerVersion
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/gpkgio/gpkgioConfigVersion.cmake"
  VERSION ${gpkgio_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT gpkgTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/gpkg/gpkgTargets.cmake"
  NAMESPACE gpkgio::
)
export(EXPORT gpkgioTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/gpkgio/gpkgioTargets.cmake"
  NAMESPACE gpkgio::
)
configure_file(cmake/gpkgConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/gpkg/gpkgConfig.cmake"
  COPYONLY
)
configure_file(cmake/gpkgioConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/gpkgio/gpkgioConfig.cmake"
  COPYONLY
)

set( ConfigPackageLocation lib/cmake/gpkg )
install( EXPORT gpkgTargets
  FILE
    gpkgTargets.cmake
  NAMESPACE
    gpkgio::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/gpkg/gpkgConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/gpkg/gpkgConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

set( ConfigPackageLocation lib/cmake/gpkgio )
install( EXPORT gpkgioTargets
  FILE
    gpkgioTargets.cmake
  NAMESPACE
    gpkgio::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/gpkgio/gpkgioConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/gpkgio/gpkgioConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
