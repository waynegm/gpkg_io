include_directories("." ${SQLITE3_INCLUDE_DIR} ${PROJECT_NAME})
add_executable(gpkgio_test gpkgio_test.cc)
add_executable(gpkg_geom_point_test gpkg_geom_point_test.cc gpkg_geom_test.cc)
add_executable(gpkg_geom_multipoint_test gpkg_geom_linestring_test.cc gpkg_geom_test.cc)
add_executable(gpkg_geom_linestring_test gpkg_geom_linestring_test.cc gpkg_geom_test.cc)
add_executable(gpkg_geom_polygon_test gpkg_geom_polygon_test.cc gpkg_geom_test.cc)

target_link_libraries(gpkgio_test PRIVATE ${SQLITE3_LIBRARY} ${PROJECT_NAME})
target_link_libraries(gpkg_geom_point_test PRIVATE ${PROJECT_NAME})
target_link_libraries(gpkg_geom_multipoint_test PRIVATE ${PROJECT_NAME})
target_link_libraries(gpkg_geom_linestring_test PRIVATE ${PROJECT_NAME})
target_link_libraries(gpkg_geom_polygon_test PRIVATE ${PROJECT_NAME})

FetchContent_Declare(
    empty_gpkg
    URL http://www.geopackage.org/data/empty.gpkg
    DOWNLOAD_NO_EXTRACT true
)

message(STATUS "Fetching empty Geopackage template for testing...")
FetchContent_MakeAvailable(empty_gpkg)

add_test(
    NAME gpkgio_test
    COMMAND $<TARGET_FILE:gpkgio_test> ${empty_gpkg_SOURCE_DIR}/empty.gpkg
)

add_test(
    NAME gpkg_geom_point_test
    COMMAND $<TARGET_FILE:gpkg_geom_point_test>
)

add_test(
    NAME gpkg_geom_linestring_test
    COMMAND $<TARGET_FILE:gpkg_geom_linestring_test>
)

add_test(
    NAME gpkg_geom_multipoint_test
    COMMAND $<TARGET_FILE:gpkg_geom_multipoint_test>
)

add_test(
    NAME gpkg_geom_polygon_test
    COMMAND $<TARGET_FILE:gpkg_geom_multipoint_test>
)
