# gpkgio

Basic C++ IO library for Geopackage vector data. Just enough to support the requirements of the OpendTect GeoPackage plugin.

## Using in a C++ project
Use CMake's FetchContent command:
```
include(FetchContent)

FetchContent_Declare(
    gpkgio
    GIT_REPOSITORY	https://github.com/waynegm/gpkg_io.git
)
FetchContent_MakeAvailable( gpkgio )
```

## Separate Build
-  Create a build folder
-  Change to the build folder
-  Run cmake ..

For a custom SQLite installation set the following CMake definitons on the CMake command line (-D...) or in the CMake GUI/TUI:
-  SQLite3_INCLUDE_DIR:PATH=
-  SQLite3_LIBRARY:FILEPATH=
