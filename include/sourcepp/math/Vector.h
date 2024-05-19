#pragma once

namespace sourcepp {

#pragma pack(push, 1)

struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct float16
{
    union float16bits
    {
        unsigned short rawWord;
        struct
        {
            unsigned short mantissa : 10;
            unsigned short biased_exponent : 5;
            unsigned short sign : 1;
        } bits;
    };

    float16bits bits;
};

struct Vector48
{
    float16 x;
    float16 y;
    float16 z;
};

#pragma pack(pop)

} // namespace sourcepp
