#include "gpkg_geom_test.h"

#include "gpkg_geom_polygon.h"

#include <iostream>

int main()
{
    std::string wktstr("POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0),(2 2, 2 6, 6 4, 2 2))");
    std::string leWKB("0103000000020000000500000000000000000000000000000000000000000000000000244000000000000000000000000000002440000000000000244000000000000000000000000000002440000000000000000000000000000000000400000000000000000000400000000000000040000000000000004000000000000018400000000000001840000000000000104000000000000000400000000000000040");
    std::string beWKB("0000000003000000020000000500000000000000000000000000000000402400000000000000000000000000004024000000000000402400000000000000000000000000004024000000000000000000000000000000000000000000000000000440000000000000004000000000000000400000000000000040180000000000004018000000000000401000000000000040000000000000004000000000000000");
    GPKGPolygon geom;
    GPKGMultiPoint mpt;
    mpt.add<GPKGMultiPoint>(0,0).add<GPKGMultiPoint>(10,0).add<GPKGMultiPoint>(10,10)
        .add<GPKGMultiPoint>(0,10).add<GPKGMultiPoint>(0,0);
    geom.add(mpt);
    mpt.clear();
    mpt.add<GPKGMultiPoint>(2,2).add<GPKGMultiPoint>(2,6).add<GPKGMultiPoint>(6,4);
    geom.add(mpt);

    mGeomTests(GPKGPolygon)

}
