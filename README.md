# gpkgio

Basic C++ IO library for Geopackage vector data. Just enough to support the requirements of the OpendTect GeoPackage plugin, i.e.
writing Point, LineString and Polygon geometry types.

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

## Need to set following:

- SQLite3_ROOT: path to SQLite3 include files and library

Default output library type is STATIC.

## Example build:

- Go to where you want to host the source, build and output
- Clone the repository: git clone https://github.com/waynegm/gpkg_io.git
- Configure and generate on Unix: cmake -S gpkg_io -B gpkg_io/build -G "Ninja Multi-Config" -DCMAKE_INSTALL_PREFIX:PATH=gpkg_io/inst -DSQLite3_ROOT=...sqlite3 -DBUILD_TESTING:option=OFF
- Configure and generate on Windows: cmake -S gpkg_io -B gpkg_io/build -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX:PATH=gpkg_io/inst -DSQLite3_ROOT=...sqlite3 -DBUILD_TESTING:option=OFF
- Build: cmake --build gpkg_io/build --config Release
- Install: cmake --install gpkg_io/build --config Release
