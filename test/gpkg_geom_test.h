#pragma once

#include "wm_endian.h"

#include <string>

class GPKGGeometry;

bool test_WKB_write(const GPKGGeometry& geom, const std::string& wkbstr, std::string& errmsg);
bool test_WKB_read(const GPKGGeometry& geom, const std::string& wkbstr, std::string& errmsg);
bool test_WKT(const GPKGGeometry& geom, const std::string& wktstr, std::string& errmsg);

#define mGeomTests(clss) \
    std::string errmsg; \
    std::string wkbstr; \
    if (SysEndian()==Endian::Little) \
	wkbstr = leWKB; \
    else \
	wkbstr = beWKB; \
    if (!test_WKB_write(geom, wkbstr, errmsg)) \
    { \
	std::cout<<"test_WKB_write for " << "clss" << " failed: got "<<errmsg<<std::endl; \
	return 1; \
    } \
    if (!test_WKB_read(geom, leWKB, errmsg)) \
    { \
	std::cout<<"test_WKB_read for LittleEndian " << "clss" << " failed: got "<<errmsg<<std::endl; \
	return 1; \
    } \
    if (!test_WKB_read(geom, beWKB, errmsg)) \
    { \
	std::cout<<"test_WKB_read for BigEndian " << "clss" << " failed: got "<<errmsg<<std::endl; \
	return 1; \
    } \
    if (!test_WKT(geom, wktstr, errmsg)) \
    { \
	std::cout<<"test_WKT " << "clss" << " failed: got "<<errmsg<<std::endl; \
	return 1; \
    } \
    return 0;
