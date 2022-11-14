add_library(${PROJECT_NAME}
    ../include/gpkgio/gpkgio.h
    utils.h
    utils.cc
    gpkgio.cc
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
	SQLite::SQLite3
	gpkg_static
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
	../include
    PRIVATE
	SQLite::SQLite3
	${libgpkg_SOURCE_DIR}/gpkg
)

target_compile_features(${PROJECT_NAME}
    PUBLIC
	cxx_std_11
	cxx_variadic_templates
)
