/*
 * Copyright (C) 2022 Wayne Mogg All rights reserved.
 *
 * This file may be used under the terms of the GNU General Public License
 * version 3 or higher, as published by the Free Software Foundation.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "utils.h"
#include "sqlite3.h"


WKBWriter::WKBWriter()
{
    wkb_writer_init(&writer_, WKB_ISO);
    error_init(&error_);
    consumer_ = wkb_writer_geom_consumer(&writer_);
    if (consumer_->begin(consumer_, &error_)!=SQLITE_OK)
	errmsg_ = error_message(&error_);
}

WKBWriter::~WKBWriter()
{
    close();
}

bool WKBWriter::add(geom_type_t geomtype, const vector<double>& coords)
{
    if (coords.size()%2)
    {
	errmsg_ = string("odd coords array size.");
	return false;
    }

    geom_header_t header;
    header.geom_type = geomtype;
    header.coord_type = GEOM_XY;
    header.coord_size = 2;

    const int numpts = coords.size()/2;
    if (consumer_->begin_geometry(consumer_, &header, &error_)!=SQLITE_OK ||
	consumer_->coordinates(consumer_, &header, numpts, coords.data(), 0, &error_)!=SQLITE_OK ||
	consumer_->end_geometry(consumer_, &header, &error_)!=SQLITE_OK)
    {
	errmsg_ = error_message(&error_);
	return false;
    }

    return true;
}

bool WKBWriter::add(const char* geomtype, const vector<double>& coords)
{
    geom_type_t geom;
    if (geom_type_from_string(geomtype, &geom)!=SQLITE_OK)
    {
	errmsg_ = string("unrecognised geomtype string: ") + geomtype;
	return false;
    }

    return add(geom, coords);;
}

bool WKBWriter::addLinearRing(const vector<double>& coords)
{
    if (!isClosedRing(coords))
    {
	errmsg_ = string("encountered unclosed ring.");
	return false;
    }

    return add(GEOM_LINEARRING, coords);
}

bool WKBWriter::addLineString(const vector<double>& coords)
{
    return add(GEOM_LINESTRING, coords);
}

bool WKBWriter::addPoint(double x, double y)
{
    std::vector<double> coords;
    coords.reserve(2);
    coords.push_back(x);
    coords.push_back(y);
    return add(GEOM_POINT, coords);
}

bool WKBWriter::addPolygon(const vector<vector<double>>& rings)
{
    geom_header_t header;
    header.geom_type = GEOM_POLYGON;
    header.coord_type = GEOM_XY;
    header.coord_size = 2;

    if (consumer_->begin_geometry(consumer_, &header, &error_)!=SQLITE_OK)
    {
	errmsg_ = error_message(&error_);
	return false;
    }

    for (const auto& ring : rings)
	if (!addLinearRing(ring))
	    return false;

    if (consumer_->end_geometry(consumer_, &header, &error_)!=SQLITE_OK)
    {
	errmsg_ = error_message(&error_);
	return false;
    }

    return true;
}

void WKBWriter::close()
{
    if (consumer_)
	consumer_->end(consumer_, &error_);

    error_destroy(&error_);
    wkb_writer_destroy(&writer_, 1);
}

string WKBWriter::getWKBString()
{
    if (consumer_)
	consumer_->end(consumer_, &error_);

    uint8_t* wkb = wkb_writer_getwkb(&writer_);
    size_t len = wkb_writer_length(&writer_);
    return string(wkb, wkb+len);
}

bool WKBWriter::isClosedRing(const vector<double>& coords)
{
    if (coords.size()%2)
    {
	errmsg_ = "odd coords array size.";
	return false;
    }

    const int sz = coords.size();
    return coords[0]==coords[sz-2] && coords[1]==coords[sz-1];
}

