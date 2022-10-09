#include "gpkg_geom_point.h"

GPKGPoint::GPKGPoint(double x, double y, int srs_id)
    : GPKGGeometry(srs_id, WkbGeomType::Point)
    , x_(x)
    , y_(y)
{}

GPKGPoint::~GPKGPoint()
{}

ByteBuffer GPKGPoint::getCoordBB() const
{
    ByteBuffer res;
    res.add(x_).add(y_);
    return res;
}

bool GPKGPoint::setCoord(const ByteBuffer& buf, size_t& wkbstrmpos)
{
    return buf.get(x_, wkbstrmpos) && buf.get(y_, wkbstrmpos);
}

std::string GPKGPoint::getCoordWKT() const
{
    std::stringstream ss;
    ss << x_ << " " << y_;
    return ss.str();
}

bool GPKGPoint::isEqual(const GPKGGeometry& other) const
{
    const auto& ocast = static_cast<const GPKGPoint&>(other);
    return x_==ocast.x_ && y_==ocast.y_;
}
