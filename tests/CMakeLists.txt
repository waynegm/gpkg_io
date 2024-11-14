include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}")
add_executable(gpkgio_test gpkgio_test.cc)

target_compile_features(gpkgio_test PRIVATE cxx_std_11)

target_link_libraries(gpkgio_test PRIVATE gpkgio gpkg_ext SQLite::SQLite3 )

FetchContent_Declare(
    empty_gpkg
    URL http://www.geopackage.org/data/empty.gpkg
    DOWNLOAD_NO_EXTRACT true
)

message(STATUS "Fetching empty Geopackage template for testing...")
FetchContent_MakeAvailable(empty_gpkg)

add_test(
    NAME GeopackageIO::isGeopackage
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg 1
)

add_test(
    NAME GeopackageIO::GeoPackageIO
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg 2
)

add_test(
    NAME GeopackageIO::hasSRS
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg 3
)

add_test(
    NAME GeopackageIO::addSRS
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg 4
)

add_test(
    NAME GeopackageIO::addPointLayer
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg 5
)

add_test(
    NAME GeopackageIO::addPolygonLayer
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg 6
)

add_test(
    NAME GeopackageIO::addLineStringLayer
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg 7
)
