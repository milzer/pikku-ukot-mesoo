#ifndef __DIESELVECTOR2_H__
#define __DIESELVECTOR2_H__

#include "DieselTypes.h"

class CDieselVector2
{
public:
    float x;
    float y;

    CDieselVector2() : x(0.0f), y(0.0f) {}
    CDieselVector2(float fX, float fY) : x(fX), y(fY) {}

    void Set(float fX, float fY)
    {
        x = fX;
        y = fY;
    }

    void Add(const CDieselVector2& v)
    {
        x += v.x;
        y += v.y;
    }

    void Mul(float f)
    {
        x *= f;
        y *= f;
    }

    float Length() const
    {
        return sqrtf(x * x + y * y);
    }

    // Scales to unit length; a zero vector stays zero
    void Normalize()
    {
        float l = Length();
        if (l > 0.0f)
        {
            Mul(1.0f / l);
        }
    }
};

#endif
