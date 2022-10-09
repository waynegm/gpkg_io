#pragma once

#include "gpkg_enums.h"
#include "bytebuffer.h"


enum class WkbGeomType : uint32_t
{
    Point = 1,
    LineString = 2,
    Polygon = 3,
    MultiPoint = 4,
    MultiLineString = 5,
    MultiPolygon = 6,
    GeometryCollection = 7,
// Z
    PointZ = 1001,
    LineStringZ = 1002,
    PolygonZ = 1003,
    MultiPointZ = 1004,
    MultiLineStringZ = 1005,
    MultiPolygonZ = 1006,
    GeometryCollectionZ = 1007,
// M
    PointM = 2001,
    LineStringM = 2002,
    PolygonM = 2003,
    MultiPointM = 2004,
    MultiLineStringM = 2005,
    MultiPolygonM = 2006,
    GeometryCollectionM = 2007,
// ZM
    PointZM = 3001,
    LineStringZM = 3002,
    PolygonZM = 3003,
    MultiPointZM = 3004,
    MultiLineStringZM = 3005,
    MultiPolygonZM = 3006,
    GeometryCollectionZM = 3007
};

struct WKBHdr
{
    uint8_t		endian;
    uint32_t		type;

};

inline const char* WkbGeomTypeToString(WkbGeomType t)
{
    switch (t)
    {
	case WkbGeomType::Point: return "POINT";
	case WkbGeomType::LineString: return "LINESTRING";
	case WkbGeomType::Polygon: return "POLYGON";
	case WkbGeomType::MultiPoint: return "MULTIPOINT";
	case WkbGeomType::MultiLineString: return "MULTILINESTRING";
	case WkbGeomType::MultiPolygon: return "MULTIPOLYGON";
	case WkbGeomType::GeometryCollection: return "GEOMETRYCOLLECTION";
	case WkbGeomType::PointZ: return "POINT Z";
	case WkbGeomType::LineStringZ: return "LINESTRING Z";
	case WkbGeomType::PolygonZ: return "POLYGON Z";
	case WkbGeomType::MultiPointZ: return "MULTIPOINT Z";
	case WkbGeomType::MultiLineStringZ: return "MULTILINESTRING Z";
	case WkbGeomType::MultiPolygonZ: return "MULTIPOLYGON Z";
	case WkbGeomType::GeometryCollectionZ: return "GEOMETRYCOLLECTION Z";
	case WkbGeomType::PointM: return "POINT M";
	case WkbGeomType::LineStringM: return "LINESTRING M";
	case WkbGeomType::PolygonM: return "POLYGON M";
	case WkbGeomType::MultiPointM: return "MULTIPOINT M";
	case WkbGeomType::MultiLineStringM: return "MULTILINESTRING M";
	case WkbGeomType::MultiPolygonM: return "MULTIPOLYGON M";
	case WkbGeomType::GeometryCollectionM: return "GEOMETRYCOLLECTION M";
	case WkbGeomType::PointZM: return "POINT ZM";
	case WkbGeomType::LineStringZM: return "LINESTRING ZM";
	case WkbGeomType::PolygonZM: return "POLYGON ZM";
	case WkbGeomType::MultiPointZM: return "MULTIPOINT ZM";
	case WkbGeomType::MultiLineStringZM: return "MULTILINESTRING ZM";
	case WkbGeomType::MultiPolygonZM: return "MULTIPOLYGON ZM";
	case WkbGeomType::GeometryCollectionZM: return "GEOMETRYCOLLECTION ZM";
    }
    return "Unknown";
}



class GPKGGeometry
{
public:
		    GPKGGeometry(int32_t srs_id, WkbGeomType gt)
			: srsid_(srs_id)
			, geomtype_(gt)
		    {}

    virtual			~GPKGGeometry();

    bool			operator==(const GPKGGeometry&) const;

    void			set_srs(int32_t srsid) { srsid_ = srsid; }

    ByteBuffer			to_gpkg();
    virtual ByteBuffer		to_wkb(bool usePrefix=true) const;
    virtual std::string		to_wkt(bool full=true) const;

    virtual std::string		getCoordWKT() const = 0;
    virtual ByteBuffer		getCoordBB() const = 0;
    virtual bool		from_wkb(const ByteBuffer&, size_t&) = 0;

    static GPKGGeometry*	create_from_wkb(const ByteBuffer&, size_t&);
    static GPKGGeometry*	create_from_gpkg(const ByteBuffer&, size_t&);

protected:
    int32_t			srsid_;
    WkbGeomType			geomtype_;

    ByteBuffer			gpkg_hdr() const;
    ByteBuffer			wkb_hdr() const;
    static ByteBuffer		make_gpkg_hdr(int);
    static ByteBuffer		make_wkb_hdr(WkbGeomType);

private:
    virtual bool		isEqual(const GPKGGeometry&) const = 0;
};
