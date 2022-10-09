#include "gpkg_io.h"
#include <iostream>

static const char* empty_gpkg = "empty.gpkg";
int main(int argc, char *argv[])
{
    std::string errmsg;
    std::string inputfile(empty_gpkg);
    if (argc>1)
	inputfile = argv[1];

    if (!GeopackageIO::isGeopackage(inputfile.c_str(), errmsg))
    {
	std::cout<<"isGeopackage failed: "<<errmsg<<std::endl;
	return 1;
    }

    {
	GeopackageIO gpkg("test.gpkg");
    }
    if (!GeopackageIO::isGeopackage("test.gpkg", errmsg))
    {
	std::cout<<"GeopackageIO(""test.gpkg"") failed: "<<errmsg<<std::endl;
	return 1;
    }
    return 0;
}
