#include "gpkg_geom_polygon.h"

GPKGPolygon::~GPKGPolygon()
{}

GPKGPolygon& GPKGPolygon::add(const GPKGMultiPoint& mpt)
{
    poly_.push_back( mpt );
    if (mpt.points().front()!=mpt.points().back())
	poly_.back().points().push_back(mpt.points().front());

    return *this;
}

ByteBuffer GPKGPolygon::to_wkb(bool usePrefix) const
{
    ByteBuffer res;
    if (usePrefix)
	res.add(wkb_hdr());

    uint32_t nrings = size();
    res.add(nrings);
    for (auto ring : poly_)
	res.add(ring.to_wkb(false));
    return res;
}

bool GPKGPolygon::from_wkb(const ByteBuffer& buf, size_t& wkbstrmpos)
{
    uint32_t nrings;
    if (!buf.get(nrings, wkbstrmpos))
	return false;
    poly_.resize(nrings);
    GPKGMultiPoint mpt;
    for (int i=0; i<nrings; i++)
    {
	if (mpt.from_wkb(buf, wkbstrmpos))
	    poly_[i] = mpt;
	else
	    return false;
    }
    return true;
}

std::string GPKGPolygon::to_wkt(bool full) const
{
    std::stringstream ss;
    if (full)
	ss << WkbGeomTypeToString(geomtype_) << " ";

    ss << "(";
    for (int i=0; i<size(); i++)
    {
	ss << poly_[i].to_wkt(false);
	if (i<size()-1)
	    ss << ",";
    }
    ss <<")";
    return ss.str();
}

bool GPKGPolygon::isEqual(const GPKGGeometry& o) const
{
    const auto& other = static_cast<const GPKGPolygon&>(o);
    return poly_==other.poly_;
}
