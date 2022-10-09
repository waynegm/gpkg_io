#pragma once

#include "gpkg_enums.h"
#include "gpkg_geom.h"
#include "gpkg_geom_multipoint.h"
#include "bytebuffer.h"
#include <vector>

class GPKGLineString : public GPKGMultiPoint
{
public:
		GPKGLineString(int srs_id=4326)
		: GPKGMultiPoint(srs_id, WkbGeomType::LineString)
		{}
    virtual	~GPKGLineString()
		{}

};

