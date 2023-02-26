cmake_minimum_required(VERSION 3.15)
project(
  gpkgio
  VERSION 1.0
  DESCRIPTION "Basic IO library for Geopackage vector data"
  LANGUAGES CXX
)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

find_package(SQLite3 REQUIRED)

add_subdirectory(external)

add_library(${PROJECT_NAME}
    src/utils.h
    src/utils.cc
    src/gpkgio.cc
)
add_library(gpkgio::gpkgio ALIAS gpkgio)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(${PROJECT_NAME}
    PUBLIC
	cxx_std_11
	cxx_variadic_templates
)
target_compile_options(${PROJECT_NAME}
    PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
	SQLite::SQLite3
	gpkg_static
)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/gpkgio)

install(TARGETS gpkgio gpkg_ext
    EXPORT gpkgio-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(gpkgio PROPERTIES EXPORT_NAME gpkgio)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT gpkgio-targets
    FILE
        gpkgioTargets.cmake
    NAMESPACE
        gpkgio::
    DESTINATION
        ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/gpkgioConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/gpkgioConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/gpkgioConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gpkgioConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/gpkgioConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT gpkgio-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/gpkgioTargets.cmake
    NAMESPACE gpkgio::)

#Register package in the User Package Registry
export(PACKAGE gpkgio)


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

