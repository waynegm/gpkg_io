#pragma once

#include "bytebuffer.h"
#include "gpkg_enums.h"
#include "gpkg_geom.h"

class GPKGPoint : public GPKGGeometry
{
public:
		GPKGPoint(double x=0.0, double y=0.0, int srs_id=4326);
    virtual	~GPKGPoint();

    GPKGPoint&		set(double x, double y)		{ x_=x; y_=y; return *this; }
    std::string		getCoordWKT() const override;
    ByteBuffer		getCoordBB() const override;
    bool		from_wkb(const ByteBuffer&, size_t&) override;

protected:
    double	x_;
    double	y_;

private:
    bool	isEqual(const GPKGGeometry&) const override;
};

