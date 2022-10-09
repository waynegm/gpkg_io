#pragma once
#include <stdint.h>

struct GeoPackageHdr
{
    uint8_t		magic[2];
    uint8_t		version;
    uint8_t		flags;
    int32_t		srsid;
};

enum class GPKGFLAG : uint8_t
{
    BigEndian = 0,
    LittleEndian = 1,
    StandardGeoPackage = 0,
    ExtendedGeoPackage = 1 << 5,
    NonEmptyGeometry = 0,
    EmptyGeometry = 1 << 4,
    NoEnvelope = 0,
    Envelope2D = 1 << 1,
    Envelope3D = 2 << 1,
    Envelope2DM = 3 << 1,
    Envelope3DM = 4 << 1
};



