#ifndef __VEC3F_H__
#define __VEC3F_H__

#include <math.h>
#include <cassert>
#include "memory.h"

namespace SimpleOBJ
{
    class Vec3f
    {
    public:

        //Constructors
        Vec3f();
        Vec3f(float x,float y, float z);
        Vec3f(const Vec3f& v);
        //Deconstructor
        virtual ~Vec3f();
    public:
        //Operators

        //Operator []
        inline float& operator [](int index)
        {
            //ASSERT(index>=0&&index<3);
            return _p[index];
        }
        inline const float& operator [](int index) const
        {
            //ASSERT(index>=0&&index<3);
            return _p[index];
        }
        
        //Operator =
        Vec3f& operator = (const Vec3f& v);

        //new add operators
        Vec3f cross(const Vec3f&);
        float product(const Vec3f&);


        //Operators +=,-=, *=, /=
        void operator +=(const Vec3f& v);
        void operator +=(float f);
        void operator -=(const Vec3f& v);
        void operator -=(float f);
        void operator *=(const Vec3f& v);
        void operator *=(float f);
        void operator /=(const Vec3f& v);
        void operator /=(float f);

        //Operators +,-.*,/
        Vec3f operator +(const Vec3f&v) const;
        Vec3f operator +(float f) const;
        Vec3f operator -(const Vec3f&v) const;
        Vec3f operator -(float f) const;
        Vec3f operator *(const Vec3f&v) const;
        Vec3f operator *(float f) const;
        Vec3f operator /(const Vec3f&v) const;
        Vec3f operator /(float f) const;

        Vec3f operator -() const;

    public:
        void Normalize();
        float L2Norm_Sqr();
     
    public:
        union
        {
            struct
            { float _p[3]; };
            struct
            { float x,y,z; };
            struct
            { float r,g,b; };
        };
        enum {_len = 3};   
        
    };
}

#endif