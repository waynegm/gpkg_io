#include "gpkg_geom_test.h"

#include "gpkg_geom_point.h"

#include <iostream>

int main()
{
    std::string wktstr("POINT (30 10)");
    std::string leWKB("01010000000000000000003E400000000000002440");
    std::string beWKB("0000000001403E0000000000004024000000000000");
    GPKGPoint geom(30.0, 10.0);

    mGeomTests(GPKGPoint)
}
