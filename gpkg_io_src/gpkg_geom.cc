#include "gpkg_geom.h"
#include "gpkg_geom_point.h"
#include "gpkg_geom_multipoint.h"
#include "gpkg_geom_linestring.h"
#include "gpkg_geom_polygon.h"
#include "wm_endian.h"

GPKGGeometry::~GPKGGeometry()
{}

bool GPKGGeometry::operator==(const GPKGGeometry& other) const
{
    if (typeid(*this)!=typeid(other))
	return false;
    return isEqual(other);
}

ByteBuffer GPKGGeometry::to_gpkg()
{
    ByteBuffer res(gpkg_hdr());
    res.add(to_wkb());
    return res;
}

ByteBuffer GPKGGeometry::make_gpkg_hdr(int32_t srsid)
{
    ByteBuffer hdr;
    const char* gp_magic = "GP";
    uint8_t	version = 0;
    uint8_t	flags = static_cast<uint8_t>(SysEndian()) |
			static_cast<uint8_t>(GPKGFLAG::NoEnvelope) |
			static_cast<uint8_t>(GPKGFLAG::StandardGeoPackage);
    hdr.add(gp_magic).add(version).add(flags).add(srsid);
    return hdr;
}

ByteBuffer GPKGGeometry::make_wkb_hdr(WkbGeomType gt)
{
    ByteBuffer hdr;
    uint8_t	endian = static_cast<uint8_t>(SysEndian());
    hdr.byte_swap(false);
    uint32_t	geomtype = static_cast<uint32_t>(gt);
    hdr.add(endian).add(geomtype);
    return hdr;
}

ByteBuffer GPKGGeometry::gpkg_hdr() const
{
    static const ByteBuffer hdr(make_gpkg_hdr(srsid_));
    return hdr;
}

ByteBuffer GPKGGeometry::wkb_hdr() const
{
    static const ByteBuffer hdr(make_wkb_hdr(geomtype_));
    return hdr;
}

GPKGGeometry* GPKGGeometry::create_from_gpkg(const ByteBuffer& bb, size_t& strmpos)
{
    strmpos = 0;
    uint8_t byte;
    if (!bb.get(byte,strmpos) || byte!=0x47)
	return nullptr;
    if (!bb.get(byte,strmpos) || byte!=0x50)
	return nullptr;
    if (!bb.get(byte,strmpos) || byte!=0)
	return nullptr;
    if (!bb.get(byte,strmpos))
	return nullptr;
    uint8_t strm_endian = byte & static_cast<uint8_t>(GPKGFLAG::LittleEndian);
    const_cast<ByteBuffer&>(bb).byte_swap(static_cast<uint8_t>(SysEndian())!=strm_endian);
    int32_t srsid;
    if (!bb.get(srsid, strmpos))
	return nullptr;
    auto* geom = create_from_wkb(bb, strmpos);
    if (geom)
	geom->set_srs(srsid);
    return geom;
}

GPKGGeometry* GPKGGeometry::create_from_wkb(const ByteBuffer& wkb, size_t& strmpos)
{
    uint8_t strm_endian;
    size_t wkbstrmpos = 0;
    if (!wkb.get(strm_endian, wkbstrmpos))
	return nullptr;
    const_cast<ByteBuffer&>(wkb).byte_swap(static_cast<uint8_t>(SysEndian())!=strm_endian);
    uint32_t gt;
    if (!wkb.get(gt, wkbstrmpos))
	return nullptr;

    WkbGeomType geomtyp = static_cast<WkbGeomType>(gt);
    GPKGGeometry* geom = nullptr;
    switch (geomtyp)
    {
	case WkbGeomType::Point:
	    geom = new GPKGPoint();
	    break;
	case WkbGeomType::LineString:
	    geom = new GPKGLineString();
	    break;
	case WkbGeomType::Polygon:
	    geom = new GPKGPolygon();
	    break;
	case WkbGeomType::MultiPoint:
	    geom = new GPKGMultiPoint();
	    break;
    }
    if (geom)
	if (geom->from_wkb(wkb, wkbstrmpos))
	    return geom;
	else
	    delete geom;

    return nullptr;
}

ByteBuffer GPKGGeometry::to_wkb(bool usePrefix) const
{
    ByteBuffer res;
    if (usePrefix)
	res.add(wkb_hdr(geomtype_));
    res.add(getCoordBB());
    return res;
}

std::string GPKGGeometry::to_wkt(bool usePrefix)
{
    std::stringstream ss;
    if (full)
	ss << WkbGeomTypeToString(geomtype_) << " ";

    ss << "(";
    ss << getCoordWKT();
    ss << ")";
    return ss.str();
}
