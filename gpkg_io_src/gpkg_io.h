#pragma once
#include <string>
#include <vector>
#include "wm_endian.h"
#include "gpkg_enums.h"

class sqlite3;
class sqlite3_stmt;



class GeopackageIO
{
public:
    GeopackageIO(const char* filename, bool append=false);
    GeopackageIO(const GeopackageIO&) = delete;
    ~GeopackageIO();

    GeopackageIO&	operator=(const GeopackageIO&) = delete;

    bool		isOK() const		{ return errmsg_.empty(); }
    std::string		errorMsg() const	{ return errmsg_; }

    bool		hasSRS(int id);
    bool		addSRS(const char* name, int id, const char* organization,
			       int org_id, const char* definition="undefined", const char* description="undefined");
    bool		hasLayer(const char*);
    bool		addLayer(const char* name, int srs_id, const char* fields_sql=nullptr);

    static bool		isGeopackage(const char* filename, std::string& errmsg);
    static const char*	gpkg_ext()	{ return "gpkg"; }

protected:
    sqlite3*		dbh_ = nullptr;
    sqlite3_stmt*	stmt_ = nullptr;
    Endian		db_endian_;
    bool		byteswap_ = false;
    std::string		filename_;
    std::string		errmsg_;

    void		open(const char* filename, bool append=false);
    void		close();
    bool		createStdTables();
    bool		sqlExecute(const char*);

    static sqlite3*	doOpen(const char* filename, int flags, std::string& errmsg);
    static int		getPragmaInt(sqlite3*, const char* pragma_name, std::string& errmsg);
    static std::vector<std::string>	sqlQueryText(sqlite3*, const char*, int col, std::string& errmsg);
    static std::vector<int>		sqlQueryInt(sqlite3*, const char*, int col, std::string& errmsg);
    static std::vector<std::string>	getTableNames(sqlite3*, std::string& errmsg);
    static bool		hasTables(sqlite3*, const std::vector<std::string>&, std::string& errmsg);

};
