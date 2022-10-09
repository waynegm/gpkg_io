#include "gpkg_geom_multipoint.h"

GPKGMultiPoint::~GPKGMultiPoint()
{}

ByteBuffer GPKGMultiPoint::getCoordBB() const
{
    ByteBuffer res;
    uint32_t npts = points_.size();
    res.add(npts);
    for (auto pt : points_)
	res.add(pt.to_wkb(false));
    return res;
}

bool GPKGMultiPoint::from_wkb(const ByteBuffer& buf, size_t& wkbstrmpos)
{
    uint32_t npts;
    if (!buf.get(npts, wkbstrmpos))
	return false;
    points_.resize(npts);
    GPKGPoint pt;
    for (int i=0; i<npts; i++)
    {
	if (pt.from_wkb(buf, wkbstrmpos))
	    points_[i] = pt;
	else
	    return false;
    }
    return true;
}

std::string GPKGMultiPoint::getCoordWKT() const
{
    std::stringstream ss;
    for (int i=0; i<size(); i++)
    {
	ss << points_[i].getCoordWKT();
	if (i<size()-1)
	    ss << ",";
    }
    return ss.str();
}

bool GPKGMultiPoint::isEqual(const GPKGGeometry& o) const
{
    const auto& other = static_cast<const GPKGMultiPoint&>(o);
    return points_==other.points_;
}
