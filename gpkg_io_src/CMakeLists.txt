add_library(${PROJECT_NAME}
    gpkg_io.h
    gpkg_io.cc
    gpkg_geom.h
    gpkg_geom.cc
    gpkg_geom_point.h
    gpkg_geom_point.cc
    gpkg_geom_multipoint.h
    gpkg_geom_multipoint.cc
    gpkg_geom_linestring.h
    gpkg_geom_polygon.h
    gpkg_geom_polygon.cc
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC	${SQLITE3_LIBRARY}
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
				$<INSTALL_INTERFACE:include>
)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES
	    gpkg_io.h
	    gpkg_enums.h
	    gpkg_geom.h
	    gpkg_geom_point.h
	    gpkg_geom_multipoint.h
	    gpkg_geom_linestring.h
	    gpkg_geom_polygon.h
	    wm_endian.h
	    bytebuffer.h
	DESTINATION include)
