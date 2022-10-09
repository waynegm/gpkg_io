#include "gpkg_io.h"
#include "sqlite3.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

GeopackageIO::GeopackageIO(const char* filename, bool append)
    : filename_(filename)
{
    open(filename_.c_str(), append);
}

GeopackageIO::~GeopackageIO()
{
    close();
}

bool GeopackageIO::hasSRS(int id)
{
    if (!dbh_)
	return false;
    string sql = "SELECT srs_id FROM gpkg_spatial_ref_sys WHERE srs_id = " + to_string(id);

    return sqlQueryInt(dbh_, sql.c_str(), 1, errmsg_).size()>0 && errmsg_.empty();
}

bool GeopackageIO::addSRS(const char* name, int id, const char* org, int org_id, const char* def, const char* desc)
{
    if (!dbh_)
	return false;

    if (hasSRS(id))
	return true;

    string sql("INSERT INTO gpkg_spatial_ref_sys"
		    "(srs_name, srs_id, organization, organization_coordsys_id, definition, description)"
		"VALUES");

    sql = sql + "('" + name + "'," + to_string(id) + ",'" + org + "'," + to_string(org_id) + ",'" + def + "','" + desc + "')";
    return sqlExecute(sql.c_str());
}

sqlite3* GeopackageIO::doOpen(const char* filename, int flags, std::string& errmsg)
{
    sqlite3* dbh;
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

	dbh = nullptr;
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

// Check PRAGMA's
    const int appID = getPragmaInt(dbh, "application_id", errmsg);
    if (appID!=0x47504b47)
    {
	if (errmsg.empty())
	    errmsg = "Invalid Application ID: " + std::to_string(appID);

	return false;
    }
    const int userVer = getPragmaInt(dbh, "user_version", errmsg);
    if( userVer!=10200)
    {
	if (errmsg.empty())
	    errmsg = "Invalid User Version: " + std::to_string(userVer);

	return false;
    }
// Check tables

    const vector<string> tables = {
					"gpkg_spatial_ref_sys",
					"gpkg_contents",
					"gpkg_geometry_columns",
					"gpkg_tile_matrix_set",
					"gpkg_tile_matrix",
					"gpkg_extensions"
				    };
    if (!hasTables(dbh, tables, errmsg))
    {
	if (errmsg.empty())
	    errmsg = "Missing one or more required tables";

	return false;
    }

    if ( dbh)
	sqlite3_close_v2(dbh);

    return true;
}

void GeopackageIO::open(const char* filename, bool append)
{
    errmsg_.clear();
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

    const char* sql = 	"PRAGMA application_id=0x47504b47;"
			"PRAGMA user_version=10200;"
			"CREATE TABLE gpkg_spatial_ref_sys ("
			    "srs_name TEXT NOT NULL,"
			    "srs_id INTEGER NOT NULL PRIMARY KEY,"
			    "organization TEXT NOT NULL,"
			    "organization_coordsys_id INTEGER NOT NULL,"
			    "definition  TEXT NOT NULL, description TEXT);"
			"CREATE TABLE gpkg_contents ("
			    "table_name TEXT NOT NULL PRIMARY KEY,"
			    "data_type TEXT NOT NULL,"
			    "identifier TEXT UNIQUE,"
			    "description TEXT DEFAULT '',"
			    "last_change DATETIME NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),"
			    "min_x DOUBLE, min_y DOUBLE, max_x DOUBLE, max_y DOUBLE,"
			    "srs_id INTEGER,"
			    "CONSTRAINT fk_gc_r_srs_id FOREIGN KEY (srs_id) REFERENCES gpkg_spatial_ref_sys(srs_id));"
			"CREATE TABLE gpkg_geometry_columns ("
			    "table_name TEXT NOT NULL,"
			    "column_name TEXT NOT NULL,"
			    "geometry_type_name TEXT NOT NULL,"
			    "srs_id INTEGER NOT NULL,"
			    "z TINYINT NOT NULL,"
			    "m TINYINT NOT NULL,"
			    "CONSTRAINT pk_geom_cols PRIMARY KEY (table_name, column_name),"
			    "CONSTRAINT fk_gc_tn FOREIGN KEY (table_name) REFERENCES gpkg_contents(table_name),"
			    "CONSTRAINT fk_gc_srs FOREIGN KEY (srs_id) REFERENCES gpkg_spatial_ref_sys (srs_id));"
			"CREATE TABLE gpkg_tile_matrix_set ("
			    "table_name TEXT NOT NULL PRIMARY KEY,"
			    "srs_id INTEGER NOT NULL,"
			    "min_x DOUBLE NOT NULL,"
			    "min_y DOUBLE NOT NULL,"
			    "max_x DOUBLE NOT NULL,"
			    "max_y DOUBLE NOT NULL,"
			    "CONSTRAINT fk_gtms_table_name FOREIGN KEY (table_name) REFERENCES gpkg_contents(table_name),"
			    "CONSTRAINT fk_gtms_srs FOREIGN KEY (srs_id) REFERENCES gpkg_spatial_ref_sys (srs_id));"
			"CREATE TABLE gpkg_tile_matrix ("
			    "table_name TEXT NOT NULL,"
			    "zoom_level INTEGER NOT NULL,"
			    "matrix_width INTEGER NOT NULL,"
			    "matrix_height INTEGER NOT NULL,"
			    "tile_width INTEGER NOT NULL,"
			    "tile_height INTEGER NOT NULL,"
			    "pixel_x_size DOUBLE NOT NULL,"
			    "pixel_y_size DOUBLE NOT NULL,"
			    "CONSTRAINT pk_ttm PRIMARY KEY (table_name, zoom_level),"
			    "CONSTRAINT fk_tmm_table_name FOREIGN KEY (table_name) REFERENCES gpkg_contents(table_name));"
			"CREATE TABLE gpkg_extensions ("
			    "table_name TEXT,"
			    "column_name TEXT,"
			    "extension_name TEXT NOT NULL,"
			    "definition TEXT NOT NULL,"
			    "scope TEXT NOT NULL,"
			    "CONSTRAINT ge_tce UNIQUE (table_name, column_name, extension_name));"
			"CREATE TABLE gpkg_ogr_contents ("
			    "table_name    TEXT NOT NULL PRIMARY KEY,"
			    "feature_count INTEGER DEFAULT NULL);";

    if (!sqlExecute(sql))
	return false;

    const char* wgs_wkt = "GEOGCS[\"WGS 84\","
				"DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],"
				"AUTHORITY[\"EPSG\",\"6326\"]],"
				"PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],"
				"UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],"
				"AUTHORITY[\"EPSG\",\"4326\"]]";

    if (!addSRS("Undefined Cartesian SRS", -1, "NONE", -1, "undefined", "undefined cartesian coordinate reference system"))
	return false;
    if (!addSRS("Undefined Geographic SRS", 0, "NONE", 0, "undefined", "undefined geographic coordinate reference system"))
	return false;
    if (!addSRS("WGS 84", 4326, "EPSG", 4326, wgs_wkt, "longitude/latitude coordinates in decimal degrees on the WGS 84 spheroid"))
	return false;

    return true;
}

bool GeopackageIO::sqlExecute(const char* sql)
{
    char* err;
    auto res = sqlite3_exec( dbh_, sql, nullptr, nullptr, &err);
    if (err)
    {
	errmsg_ += err;
	sqlite3_free(err);
	return false;
    }
    return true;
}

