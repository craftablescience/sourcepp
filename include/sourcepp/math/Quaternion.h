#pragma once

namespace sourcepp {

#pragma pack(push, 1)

struct Quaternion {
	float w;
	float x;
	float y;
	float z;
};

struct Quaternion48
{
    unsigned short x:16;
    unsigned short y:16;
    unsigned short z:15;
    unsigned short wneg:1;
};

struct Quaternion64
{
    uint64_t x:21;
    uint64_t y:21;
    uint64_t z:21;
    uint64_t wneg:1;
};

#pragma pack(pop)

} // namespace sourcepp
