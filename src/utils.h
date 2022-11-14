#pragma once
/*
 * Copyright (C) 2022 Wayne Mogg All rights reserved.
 *
 * This file may be used under the terms of the GNU General Public License
 * version 3 or higher, as published by the Free Software Foundation.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <vector>
#include <string>

extern "C" {
#include "wkb.h"
#include "error.h"
#include "geomio.h"
}


using namespace std;

inline string joinStrings(const vector<string>& strs, const string sep=",")
{
    string out = strs[0];
    for(unsigned int i = 1; i < strs.size(); i++) {
        out += sep + strs[i];
    }
    return out;
}

class WKBWriter
{
public:
    WKBWriter();
    ~WKBWriter();

    bool		isOK() const		{ return errmsg_.empty(); }
    string		errMsg() const		{ return errmsg_; }

    string		getWKBString();

    bool		addLineString(const vector<double>&);
    bool		addPoint(double x, double y);
    bool		addPolygon(const vector<vector<double>>& rings);


protected:
    bool		isClosedRing(const vector<double>&);

    bool		addLinearRing(const vector<double>&);
    bool		add(geom_type_t type, const vector<double>&);
    bool		add(const char* type, const vector<double>&);

    wkb_writer_t	writer_;
    errorstream_t	error_;
    geom_consumer_t*	consumer_;

    string		errmsg_;

private:
    void		close();

};
