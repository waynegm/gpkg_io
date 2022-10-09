#include "gpkg_geom_test.h"

#include "gpkg_geom_linestring.h"

#include <iostream>

int main()
{
    std::string wktstr("LINESTRING (1 2,3 4)");
    std::string leWKB("010200000002000000000000000000F03F000000000000004000000000000008400000000000001040");
    std::string beWKB("0000000002000000023FF0000000000000400000000000000040080000000000004010000000000000");
    GPKGLineString geom;
    geom.add<GPKGLineString>(1,2).add<GPKGLineString>(3,4);

    mGeomTests(GPKGLineString)

}
