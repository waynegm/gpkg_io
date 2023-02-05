#include "gpkgio/gpkgio.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <algorithm>

extern "C" {
#include "wkb.h"
#include "error.h"
#include "geomio.h"
}

using namespace std;




string GeopackageIO::gpkglibloc_ = std::string();

GeopackageIO::GeopackageIO(const char* filename, bool append)
    : filename_(filename)
    , append_(append)
{
    open(filename_.c_str(), append);
}

GeopackageIO::~GeopackageIO()
{
    close();
}

bool GeopackageIO::hasSRS(int id)
{
    return hasSRS(to_string(id).c_str());
}

bool GeopackageIO::hasSRS(const char* id)
{
    if (!dbh_)
	return false;
    string sql = "SELECT srs_id FROM gpkg_spatial_ref_sys WHERE srs_id = ";
    sql += id;

    return sqlQueryInt(dbh_, sql.c_str(), 1, errmsg_).size()>0 && errmsg_.empty();
}

bool GeopackageIO::addSRS(const char* name, int id, const char* org, int org_id, const char* def, const char* desc)
{
    return addSRS(name, to_string(id).c_str(), org, to_string(org_id).c_str(), def, desc);
}

bool GeopackageIO::addSRS(const char* name, const char* id, const char* org, const char* org_id, const char* def, const char* desc)
{
    if (!dbh_)
	return false;

    if (hasSRS(id))
	return true;

    string sql("INSERT INTO gpkg_spatial_ref_sys"
		    "(srs_name, srs_id, organization, organization_coordsys_id, definition, description)"
		"VALUES");

    sql = sql + "('" + name + "'," + id + ",'" + org + "'," + org_id + ",'" + def + "','" + desc + "')";
    return sqlExecute(sql.c_str());
}

sqlite3* GeopackageIO::doOpen(const char* filename, int flags, std::string& errmsg)
{
    sqlite3* dbh;
    if (!gpkglibloc_.c_str())
    {
	errmsg = "Location of libgpkg SQLite extension must be defined.";
	return nullptr;
    }

    auto res = sqlite3_open_v2(filename, &dbh, flags, nullptr);
    if (res!=SQLITE_OK)
    {
	if ( dbh)
	{
	    errmsg = sqlite3_errmsg(dbh);
	    sqlite3_close_v2(dbh);
	}
	else
	    errmsg = "Cannot open database: " + std::string(filename);

	return nullptr;
    }

    if ( sqlite3_enable_load_extension(dbh, 1)!=SQLITE_OK ||
	      sqlite3_load_extension(dbh, gpkglibloc_.c_str(), 0, nullptr)
	      !=SQLITE_OK )
    {
	errmsg = sqlite3_errmsg(dbh);
	sqlite3_close_v2(dbh);
	return nullptr;
    }

    return dbh;
}

int GeopackageIO::getPragmaInt(sqlite3* dbh, const char* pragma_name, std::string& errmsg)
{
    int result = -1;
    std::string sql = "PRAGMA " + std::string(pragma_name);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(dbh, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
	errmsg += sqlite3_errmsg(dbh);
	return result;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
        errmsg += sqlite3_errmsg(dbh);
        sqlite3_finalize(stmt);
        return result;
    }
    if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
	errmsg += "not found";
        return result;
    }

    result = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return result;

}

std::vector<std::string> GeopackageIO::sqlQueryText(sqlite3* dbh, const char* sql, int col, std::string& errmsg)
{
    vector<string> res;
    if (!dbh)
	return res;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(dbh, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
	errmsg += sqlite3_errmsg(dbh);
    else
    {
	for (;;)
	{
	    rc = sqlite3_step(stmt);
	    if (rc == SQLITE_ROW)
		res.push_back(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, col))));
	    else if (rc == SQLITE_DONE)
		break;
	    else
	    {
		errmsg += sqlite3_errmsg(dbh);
		break;
	    }
	}
    }

    sqlite3_finalize(stmt);
    return res;
}

std::vector<int> GeopackageIO::sqlQueryInt(sqlite3* dbh, const char* sql, int col, std::string& errmsg)
{
    vector<int> res;
    if (!dbh)
	return res;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(dbh, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
	errmsg += sqlite3_errmsg(dbh);
    else
    {
	for (;;)
	{
	    rc = sqlite3_step(stmt);
	    if (rc == SQLITE_ROW)
		res.push_back(sqlite3_column_int(stmt, col));
	    else if (rc == SQLITE_DONE)
		break;
	    else
	    {
		errmsg += sqlite3_errmsg(dbh);
		break;
	    }
	}
    }

    sqlite3_finalize(stmt);
    return res;
}

std::vector<std::string> GeopackageIO::getTableNames(sqlite3* dbh, string& errmsg)
{
    return sqlQueryText(dbh, "PRAGMA table_list", 1, errmsg);
}

bool GeopackageIO::hasTables(sqlite3* dbh, const vector<string>& tables, std::string& errmsg)
{
    if (!dbh)
	return false;

    const auto ftab = getTableNames(dbh, errmsg);
    if (!errmsg.empty() || ftab.size()<tables.size())
	return false;
    for (const auto table : tables)
    {
	if (std::find(ftab.begin(), ftab.end(), table)==ftab.end())
	    return false;
    }
    return true;
}

bool GeopackageIO::isGeopackage(const char* filename, std::string& errmsg)
{
    sqlite3* dbh = doOpen(filename, SQLITE_OPEN_READONLY, errmsg);
    if (!dbh || !errmsg.empty())
	return false;

    const char* sql = "SELECT CheckSpatialMetadata();";
    bool result = sqlExecute(dbh, sql, errmsg);
    if (dbh)
	sqlite3_close_v2(dbh);

    return result;
}

void GeopackageIO::open(const char* filename, bool append)
{
    errmsg_.clear();
    if (append && !isGeopackage(filename, errmsg_))
	return;

    int flags = append ? SQLITE_OPEN_READWRITE : SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    if (dbh_)
	close();

    dbh_ = doOpen(filename, flags, errmsg_);
    if (!dbh_)
	return;

    if (!append && !createStdTables() )
	close();
}

void GeopackageIO::close()
{
    if (dbh_)
	sqlite3_close_v2(dbh_);

    dbh_ = nullptr;
}

bool GeopackageIO::createStdTables()
{
    if (!dbh_)
	return false;

    const char* sql = "SELECT InitSpatialMetadata();";

    if (!sqlExecute(sql))
	return false;

    return true;
}

bool GeopackageIO::sqlExecute(const char* sql)
{
    return dbh_ ? sqlExecute(dbh_, sql, errmsg_) : false;
}

bool GeopackageIO::sqlExecute(sqlite3* db, const char* sql, std::string& errmsg)
{
    char* err;
    auto res = sqlite3_exec( db, sql, nullptr, nullptr, &err);
    if (err)
    {
	errmsg += err;
	sqlite3_free(err);
	return false;
    }
    return true;
}

void GeopackageIO::setGPKGlib_location(const char* libloc)
{
    gpkglibloc_ = libloc;
}

bool GeopackageIO::hasLayer(const char* name)
{
    if (!dbh_)
	return false;

    const auto ftab = getTableNames(dbh_, errmsg_);
    if (!errmsg_.empty())
	return false;

    if (std::find(ftab.begin(), ftab.end(), name)==ftab.end())
	return false;

    return true;
}

bool GeopackageIO::addGeomLayer(const char* type, const char* layernm, int srs_id,
				 const vector<string>& field_names,
				 const vector<string>& field_defs)
{
    return addGeomLayer(type, layernm, to_string(srs_id).c_str(), field_names, field_defs);
}

bool GeopackageIO::addGeomLayer(const char* type, const char* layernm, const char* srs_id,
				 const vector<string>& field_names,
				 const vector<string>& field_defs)
{
    errmsg_.clear();
    if (!makeGeomTable(layernm, srs_id, type, field_names, field_defs))
    {
	errmsg_.insert(0, "addGeomLayer: ");
	return false;
    }

    return true;
}

bool GeopackageIO::checkFieldInfo(const vector<string>& field_names,
				  const vector<string>& field_defs)
{
    if (field_names.size()!=field_defs.size())
    {
	errmsg_ = "mismatch in field names and definitions.";
	return false;
    }
    return true;
}

bool GeopackageIO::makeGeomTable(const char* layernm, const char* srsid, const char* type,
				 const vector<string>& field_names,
				 const vector<string>& field_defs)
{
    if (!checkFieldInfo(field_names, field_defs))
	return false;

    if (!hasLayer(layernm))
    {
	string fields("fid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL");
	for (int idx=0; idx<field_names.size(); idx++)
	    fields += string(",") + field_names[idx] + " " + field_defs[idx];

	string sql =	string("SAVEPOINT create_") + layernm + ";" +
			"PRAGMA foreign_keys = false;" +
			"CREATE TABLE " + layernm + " (" + fields + ");" +
			"SELECT AddGeometryColumn('" + layernm + "', 'geom','" +
			type + "', " + srsid + ", 0, 0);" +
			"INSERT INTO gpkg_contents (table_name, data_type, identifier, srs_id)" +
			"values ('" + layernm + "', 'features', '" + layernm + "','"+ srsid + "');" +
			"SELECT CreateSpatialIndex('" + layernm + "', 'geom', 'rowid');" +
			"PRAGMA foreign_keys = true;" +
			"PRAGMA foreign_key_check('gpkg_contents');" +
			"PRAGMA foreign_key_check('gpkg_geometry_columns');" +
			"RELEASE create_" + layernm + ";";
	return sqlExecute(sql.c_str());
    }

    return true;
}

bool GeopackageIO::makeGeomStatement(sqlite3_stmt** stmt,  const char* layernm, int srsid,
				     const vector<string>& field_names)
{
    return makeGeomStatement(stmt, layernm, to_string(srsid).c_str(), field_names);
}

bool GeopackageIO::makeGeomStatement(sqlite3_stmt** stmt,  const char* layernm, const char* srsid,
				     const vector<string>& field_names)
{
    errmsg_.clear();
    if (!hasLayer(layernm))
	return false;

    string fields("geom");
    if (!field_names.empty())
	fields += string(",") + joinStrings(field_names);

    string values("ST_GeomFromWKB(?,");
    values += string(srsid) + ")";
    for (const auto name : field_names )
	values += ",?";

    string sql =    string("INSERT INTO ") + layernm + "(" + fields + ")" +
		    " VALUES " + "(" + values + ");";

    if (sqlite3_prepare_v2(dbh_, sql.c_str(), -1, stmt, NULL)!=SQLITE_OK)
    {
	errmsg_ = "preparing geometry insert statement failed.";
	return false;
    }
    return true;
}

string GeopackageIO::getPointWKB(double x, double y)
{

    WKBWriter wkbwr;
    wkbwr.addPoint(x, y);
    if (!wkbwr.isOK())
    {
	errmsg_ = wkbwr.errMsg();
	return string();
    }

    return wkbwr.getWKBString();
}

string GeopackageIO::getLineStringWKB(const std::vector<double>& coords)
{
    WKBWriter wkbwr;
    wkbwr.addLineString(coords);
    if (!wkbwr.isOK())
    {
	errmsg_ = wkbwr.errMsg();
	return string();
    }

    return wkbwr.getWKBString();
}

string GeopackageIO::getPolygonWKB(const std::vector<std::vector<double>>& rings)
{
    WKBWriter wkbwr;
    wkbwr.addPolygon(rings);
    if (!wkbwr.isOK())
    {
	errmsg_ = wkbwr.errMsg();
	return string();
    }

    return wkbwr.getWKBString();
}

bool GeopackageIO::startTransaction()
{
    return sqlExecute("BEGIN TRANSACTION");
}

bool GeopackageIO::commitTransaction()
{
    return sqlExecute("END TRANSACTION");
}

bool GeopackageIO::rollbackTransaction()
{
    return sqlExecute("ROLLBACK");
}

void GeopackageIO::bind_value(sqlite3_stmt* stmt, int col, int val)
{
    sqlite3_bind_int(stmt, col, val);
}

void GeopackageIO::bind_value(sqlite3_stmt* stmt, int col, float val)
{
    sqlite3_bind_double(stmt, col, static_cast<double>(val));
}

void GeopackageIO::bind_value(sqlite3_stmt* stmt, int col, double val)
{
    sqlite3_bind_double(stmt, col, val);
}

void GeopackageIO::bind_value(sqlite3_stmt* stmt, int col, std::string& val)
{
    if (!val.empty())
	sqlite3_bind_text(stmt, col, val.data(), static_cast<int>(val.size()), SQLITE_TRANSIENT);
}

void GeopackageIO::bind_value(sqlite3_stmt* stmt, int col, const char* val)
{
    if (val)
	sqlite3_bind_text(stmt, col, val, static_cast<int>(strlen(val)), SQLITE_TRANSIENT);
}




