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

#include <string>
#include <vector>
#include "sqlite3.h"

class GeopackageIO
{
public:
    GeopackageIO(const char* filename, bool append=false);
    GeopackageIO(const GeopackageIO&) = delete;
    ~GeopackageIO();

    GeopackageIO&	operator=(const GeopackageIO&) = delete;

    bool		isOK() const		{ return errmsg_.empty(); }
    const char*		errorMsg() const	{ return errmsg_.c_str(); }
    sqlite3*		dbHandle() const	{ return dbh_; }

    bool		hasSRS(int id);
    bool		hasSRS(const char* id);
    bool		addSRS(const char* name, int srsid, const char* organization,
			       int org_id, const char* definition="undefined", const char* description="undefined");
    bool		addSRS(const char* name, const char* srsid, const char* organization,
			       const char* org_id, const char* definition="undefined", const char* description="undefined");
    bool		hasLayer(const char*);

    bool		addGeomLayer(const char* type, const char* layername, int srsid,
				      const std::vector<std::string>& field_names,
				      const std::vector<std::string>& field_defs);
    bool		addGeomLayer(const char* type, const char* layername, const char* srsid,
				      const std::vector<std::string>& field_names,
				      const std::vector<std::string>& field_defs);

    bool		startTransaction();
    bool		commitTransaction();
    bool		rollbackTransaction();

    template<typename... Args>
    bool		addLineString(sqlite3_stmt* stmt, const std::vector<double>& points, Args... args);
    template<typename... Args>
    bool		addPoint(sqlite3_stmt* stmt, double x, double y, Args... args);
    template<typename... Args>
    bool		addPolygon(sqlite3_stmt* stmt, const std::vector<std::vector<double>>& rings, Args... args);

    bool		makeGeomStatement(sqlite3_stmt** stmt, const char* layernm, int srsid,
					  const std::vector<std::string>& field_names);
    bool		makeGeomStatement(sqlite3_stmt** stmt, const char* layernm, const char* srsid,
					  const std::vector<std::string>& field_names);

    static bool		isGeopackage(const char* filename, std::string& errmsg);
    static const char*	gpkg_ext()	{ return "gpkg"; }
    static void		setGPKGlib_location(const char*);
    static bool		sqlExecute(sqlite3*, const char*, std::string& errmsg);
    static int		getPragmaInt(sqlite3*, const char* pragma_name, std::string& errmsg);
    static std::vector<std::string>	sqlQueryText(sqlite3*, const char*, int col, std::string& errmsg);
    static std::vector<int>		sqlQueryInt(sqlite3*, const char*, int col, std::string& errmsg);
    static std::vector<std::string>	getTableNames(sqlite3*, std::string& errmsg);
    static bool		hasTables(sqlite3*, const std::vector<std::string>&, std::string& errmsg);

protected:
    sqlite3*		dbh_ = nullptr;
    sqlite3_stmt*	stmt_ = nullptr;
    std::string		filename_;
    bool		append_;
    std::string		errmsg_;
    static std::string	gpkglibloc_;

    void		open(const char* filename, bool append=false);
    void		close();
    bool		createStdTables();
    bool		sqlExecute(const char*);
    bool		checkFieldInfo(const std::vector<std::string>& field_names,
				       const std::vector<std::string>& field_defs);
    bool		makeGeomTable(const char* layernm, const char* srsid, const char* type,
				      const std::vector<std::string>& field_names,
				      const std::vector<std::string>& field_defs);

    std::string		getLineStringWKB(const std::vector<double>&);
    std::string		getPointWKB(double x, double y);
    std::string		getPolygonWKB(const std::vector<std::vector<double>>&);

    void		bind_values(sqlite3_stmt* stmt, int col)	{}
    template <typename Arg, typename... ArgRest>
    void		bind_values(sqlite3_stmt* stmt, int col, Arg val, const ArgRest&... rest);

    void		bind_value(sqlite3_stmt* stmt, int col, int val);
    void		bind_value(sqlite3_stmt* stmt, int col, float val);
    void		bind_value(sqlite3_stmt* stmt, int col, double val);
    void		bind_value(sqlite3_stmt* stmt, int col, std::string& val);
    void		bind_value(sqlite3_stmt* stmt, int col, const char* val);
    template <typename Arg>
    void		bind_value(sqlite3_stmt* stmt, int col, Arg val);

    template <typename... Args>
    bool		addGeom(sqlite3_stmt* stmt, const std::string& wkbstr, Args... args);


    static sqlite3*	doOpen(const char* filename, int flags, std::string& errmsg);

};


template <typename Arg, typename... ArgRest>
void GeopackageIO::bind_values(sqlite3_stmt* stmt, int col, Arg val, const ArgRest&... rest)
{
    bind_value(stmt, col, val);
    bind_values(stmt, col+1, rest...);
}

template <typename Arg>
void GeopackageIO::bind_value(sqlite3_stmt* stmt, int col, Arg val)
{}


template<typename... Args>
bool GeopackageIO::addLineString(sqlite3_stmt* stmt, const std::vector<double>& points, Args... args)
{
    errmsg_.clear();
    std::string wkbstr = getLineStringWKB(points);
    if (!isOK() || !addGeom(stmt, wkbstr, args... ))
	errmsg_.insert(0, "addLineString: ");

    return isOK();
}

template<typename... Args>
bool GeopackageIO::addPoint(sqlite3_stmt* stmt, double x, double y, Args... args)
{
    errmsg_.clear();
    std::string wkbstr = getPointWKB(x, y);
    if (!isOK() || !addGeom(stmt, wkbstr, args... ))
	errmsg_.insert(0, "addPoint: ");

    return isOK();
}

template<typename... Args>
bool GeopackageIO::addPolygon(sqlite3_stmt* stmt, const std::vector<std::vector<double>>& rings, Args... args)
{
    errmsg_.clear();
    std::string wkbstr = getPolygonWKB(rings);
    if (!isOK() || !addGeom(stmt, wkbstr, args... ))
	errmsg_.insert(0, "addPolygon: ");

    return isOK();
}

template<typename... Args>
bool GeopackageIO::addGeom(sqlite3_stmt* stmt, const std::string& wkbstr, Args... args)
{
    errmsg_.clear();
    sqlite3_reset(stmt);
    sqlite3_bind_blob(stmt, 1, wkbstr.data(), static_cast<int>(wkbstr.size()), SQLITE_STATIC);
    bind_values(stmt, 2, args...);
    if (sqlite3_step(stmt)!=SQLITE_DONE)
	errmsg_ = "binding values while adding geometry failed.";

    return isOK();
}
