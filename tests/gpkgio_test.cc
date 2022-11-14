#include "gpkgio/gpkgio.h"
#include <iostream>
#include <cstdio>

static const char* empty_gpkg = "empty.gpkg";

std::vector<std::string> getGeomAsWKT(sqlite3* dbh, std::string& errmsg,
				      const char* layernm, const char* colnm="geom")
{
    std::vector<std::string> res;
    if (!dbh)
	return res;

    std::string sql = std::string("SELECT AsText(") + colnm + ") FROM " + layernm + ";";
    res = GeopackageIO::sqlQueryText(dbh, sql.c_str(), 0, errmsg);
    return res;
}

int main(int argc, char *argv[])
{
    std::string errmsg;
    std::string inputfile(empty_gpkg);
    int testid = 1;
    GeopackageIO::setGPKGlib_location("../install/bin/libgpkg.so");
    if (argc>1)
	inputfile = argv[1];

    if (argc>2)
	testid = std::stoi(argv[2]);

    switch (testid)
    {
	case 1:
	    if (!GeopackageIO::isGeopackage(inputfile.c_str(), errmsg))
	    {
		std::cout<<errmsg<<std::endl;
		return 1;
	    }
	    break;
	case 2:
	{
	    std::remove("test.gpkg");
	    GeopackageIO gpkg("test.gpkg");
	    if (!gpkg.isOK())
	    {
		std::cout<<gpkg.errorMsg()<<std::endl;
		return 1;
	    }
	    if (!GeopackageIO::isGeopackage("test.gpkg", errmsg))
	    {
		std::cout<<errmsg<<std::endl;
		return 1;
	    }
	    break;
	}
	case 3:
	{
	    std::remove("test.gpkg");
	    GeopackageIO gpkg("test.gpkg");
	    if (!gpkg.hasSRS(-1) || !gpkg.hasSRS(0) || !gpkg.hasSRS(4326))
	    {
		std::cout<< gpkg.errorMsg() << std::endl;
		return 1;
	    }
	    break;
	}
	case 4:
	{
	    std::remove("test.gpkg");
	    GeopackageIO gpkg("test.gpkg");
	    if (!gpkg.addSRS("Test", 23031, "EPSG", 23031) ||!gpkg.hasSRS(23031) ||!gpkg.isOK())
	    {
		std::cout<< gpkg.errorMsg() << std::endl;
		return 1;
	    }
	    break;
	}
	case 5:
	{
	    std::remove("test.gpkg");
	    GeopackageIO gpkg("test.gpkg");
	    std::vector<std::string> fieldnms({"name"});
	    std::vector<std::string> fielddefs({"TEXT NOT NULL"});

	    sqlite3_stmt* stmt;
	    int srsid = 0;
	    if (!gpkg.addGeomLayer("point", "points", srsid, fieldnms, fielddefs) ||
		!gpkg.makeGeomStatement(&stmt, "points", srsid, fieldnms) ||
		!gpkg.addPoint(stmt, 20.0, 40.0, "test"))
	    {
		std::cout<< gpkg.errorMsg() << std::endl;
		return 1;
	    }
	    sqlite3_finalize(stmt);

	    std::string errmsg;
	    std::string expected("Point (20 40)");
	    auto res = getGeomAsWKT(gpkg.dbHandle(), errmsg, "points");
	    if (res[0]!=expected)
	    {
		std::cout<< errmsg << std::endl;
		std::cout<< "Got: " << res[0] << " Expected: " << expected << std::endl;
		return 1;
	    }

	    break;
	}
	case 6:
	{
	    std::remove("test.gpkg");
	    GeopackageIO gpkg("test.gpkg");
	    std::vector<std::string> fieldnms({"name"});
	    std::vector<std::string> fielddefs({"TEXT NOT NULL"});

	    sqlite3_stmt* stmt;
	    int srsid = 0;
	    std::vector<double> outerring({10, 20, 30, 20, 40, 40, 10, 40, 10, 20});
	    std::vector<double> innerring({20, 25, 30, 25, 30, 30, 20, 25});
	    std::vector<std::vector<double>> poly({outerring,innerring});
	    if (!gpkg.addGeomLayer("polygon", "polygons", srsid, fieldnms, fielddefs) ||
		!gpkg.makeGeomStatement(&stmt, "polygons", srsid, fieldnms) ||
		!gpkg.addPolygon(stmt, poly, "test"))
	    {
		std::cout<< gpkg.errorMsg() << std::endl;
		return 1;
	    }
	    sqlite3_finalize(stmt);

	    std::string errmsg;
	    std::string expected("Polygon ((10 20, 30 20, 40 40, 10 40, 10 20), (20 25, 30 25, 30 30, 20 25))");
	    auto res = getGeomAsWKT(gpkg.dbHandle(), errmsg, "polygons");
	    if (res[0]!=expected)
	    {
		std::cout<< errmsg << std::endl;
		std::cout<< "Got: " << res[0] << " Expected: " << expected << std::endl;
		return 1;
	    }

	    break;
	}
	case 7:
	{
	    std::remove("test.gpkg");
	    GeopackageIO gpkg("test.gpkg");
	    std::vector<std::string> fieldnms({"name"});
	    std::vector<std::string> fielddefs({"TEXT NOT NULL"});

	    sqlite3_stmt* stmt;
	    int srsid = 0;
	    std::vector<double> points({10, 20, 30, 20, 40, 40, 10, 40});
	    if (!gpkg.addGeomLayer("linestring", "lines", srsid, fieldnms, fielddefs) ||
		!gpkg.makeGeomStatement(&stmt, "lines", srsid, fieldnms) ||
		!gpkg.addLineString(stmt, points, "test"))
	    {
		std::cout<< gpkg.errorMsg() << std::endl;
		return 1;
	    }
	    sqlite3_finalize(stmt);

	    std::string errmsg;
	    std::string expected("LineString (10 20, 30 20, 40 40, 10 40)");
	    auto res = getGeomAsWKT(gpkg.dbHandle(), errmsg, "lines");
	    if (res[0]!=expected)
	    {
		std::cout<< errmsg << std::endl;
		std::cout<< "Got: " << res[0] << " Expected: " << expected << std::endl;
		return 1;
	    }

	    break;
	}
	default:
	    std::cout<<"Unrecognised testid: "<< testid << std::endl;
	    return 1;
    }

    return 0;
}
