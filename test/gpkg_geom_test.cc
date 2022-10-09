#include "gpkg_geom_test.h"

#include "bytebuffer.h"
#include "gpkg_geom.h"
#include <string>

bool test_WKB_write(const GPKGGeometry& geom, const std::string& wkbstr, std::string& errmsg)
{
    errmsg.clear();
    ByteBuffer bb = geom.to_wkb();
    const bool res = bb.toHex()==wkbstr;
    if (!res)
    {
	errmsg = bb.toHex();
	errmsg += " expected ";
	errmsg += wkbstr;
    }
    return res;
}

bool test_WKB_read(const GPKGGeometry& geom, const std::string& wkbstr, std::string& errmsg)
{
    errmsg.clear();
    ByteBuffer bb;
    bb.fromHex(wkbstr);
    size_t pos = 0;
    GPKGGeometry* newgeom = GPKGGeometry::create_from_wkb(bb, pos);
    const bool res = geom==*newgeom;
    if (!res)
    {
	errmsg = newgeom->to_wkt();
	errmsg += " expected ";
	errmsg += geom.to_wkt();
    }
    return res;
}

bool test_WKT(const GPKGGeometry& geom, const std::string& wktstr, std::string& errmsg)
{
    errmsg.clear();
    const bool res = geom.to_wkt()==wktstr;
    if (!res)
    {
	errmsg = geom.to_wkt();
	errmsg += " expected ";
	errmsg += wktstr;
    }
    return res;
}
