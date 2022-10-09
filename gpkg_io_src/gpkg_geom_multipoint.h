#pragma once

#include "gpkg_enums.h"
#include "gpkg_geom.h"
#include "gpkg_geom_point.h"
#include "bytebuffer.h"
#include <vector>

class GPKGMultiPoint : public GPKGGeometry
{
public:
		GPKGMultiPoint(int srs_id=4326, WkbGeomType gt=WkbGeomType::MultiPoint )
		: GPKGGeometry(srs_id, gt)
		{}
    virtual	~GPKGMultiPoint();

    template <class T>
    T&			add(const GPKGPoint&);
    template <class T>
    T&			add(double x, double y);

    size_t		size() const		{ return points_.size(); }
    void		clear()			{ points_.clear(); }
    std::vector<GPKGPoint>&	points()	{ return points_; }

    ByteBuffer		getCoordBB() const override;
    std::string		getCoordWKT() const override;
    bool		from_wkb(const ByteBuffer&, size_t&) override;

protected:
    std::vector<GPKGPoint>	points_;

private:
    bool		isEqual(const GPKGGeometry&) const override;
};

template <class T>
T& GPKGMultiPoint::add(const GPKGPoint& pt)
{
    points_.push_back(pt);
    return static_cast<T&>(*this);
}

template <class T>
T& GPKGMultiPoint::add(double x, double y)
{
    return add<T>(GPKGPoint(x, y));
}
