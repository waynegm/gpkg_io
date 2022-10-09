#pragma once

#include "gpkg_enums.h"
#include "gpkg_geom.h"
#include "gpkg_geom_multipoint.h"
#include "bytebuffer.h"
#include <vector>

class GPKGPolygon : public GPKGGeometry
{
public:
		GPKGPolygon(int srs_id=4326, WkbGeomType gt=WkbGeomType::Polygon)
		: GPKGGeometry(srs_id, gt)
		{}
		~GPKGPolygon();

    GPKGPolygon&	add(const GPKGMultiPoint&);

    size_t		size() const	{ return poly_.size(); }
    void		clear()		{ poly_.clear(); }

    ByteBuffer		to_wkb(bool usePrefix=true) const override;
    std::string		to_wkt(bool full=true) const override;
    bool		from_wkb(const ByteBuffer&, size_t&) override;

protected:
    std::vector<GPKGMultiPoint>	poly_;

private:
    bool		isEqual(const GPKGGeometry&) const override;

};

