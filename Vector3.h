#ifndef CSE168_VECTOR3_H_INCLUDED
#define CSE168_VECTOR3_H_INCLUDED

#include "TVector.h"

#include <math.h>
#include <float.h>
#include <iostream>

#ifdef WIN32
#pragma warning(disable:4305) // disable useless warnings
#pragma warning(disable:4244)
#endif

typedef TVector<3> Vector3;

#define Vector3 Vector3v

class Vector3
{

public:
    float _x, _y, _z;      // The x & y & z coordinates.

    Vector3() :
        _x(0), _y(0), _z(0) {}

    Vector3(float s) :
        _x(s), _y(s), _z(s) {}

    Vector3(float xVal, float yVal, float _zVal) :
        _x(xVal), _y(yVal), _z(_zVal) {}

    float x() const { return _x; }
    float y() const { return _y; }
    float z() const { return _z; }

    //! Assignment operator.
    /*!
        Assigns the values from \a a to this Vec3.
    */
    const Vector3 & operator=(const Vector3& a) {_x = a._x; _y = a._y; _z = a._z; return *this;}
    
    //! Assignment operator.
    /*!
        Sets all components of this Vec3 to \a a.
    */
    const Vector3 & operator=(float a) {_x = _y = _z = a; return *this;}

    void set(float a) {_x = _y = _z = a;}
    void set(float a, float b, float c) {_x = a; _y = b; _z = c;}
    void set(const Vector3 & v) {_x = v._x; _y = v._y; _z = v._z;}
    
    
    //! Access operator.        
    /*!
        Returns the ith component of the vector.
        \param i The component to return.
        \warning i must be either 0, 1, or 2 in order to get expected results.
    */
    float & operator[](int i) {return (&_x)[i];}
    
    //! Constant access operator.
    /*!
        Returns the ith component of a constant vector.
        \param i The component to return.
        \warning i must be either 0, 1, or 2 in order to get expected results.
    */
    const float & operator[](int i) const {return (&_x)[i];}


    //! Component-wise vector addition operator.
    Vector3 operator+(const Vector3& v) const
    {
        return Vector3(_x + v._x, _y + v._y, _z + v._z);
    }
    
    //! Component-wise vector addition-assignment operator.
    const Vector3 & operator+=(const Vector3& v)
    {
        _x += v._x; _y += v._y; _z += v._z; return *this;
    }

    //! Scalar addition-assignment operator.
    const Vector3 & operator+=(float a) {_x += a; _y += a; _z += a; return *this;}


    //! Component-wise vector subtraction operator.
    Vector3 operator-(const Vector3& v) const
    {
        return Vector3(_x - v._x, _y - v._y, _z - v._z);
    }
    
    //! Component-wise vector subtraction-assignment operator.
    const Vector3 & operator-=(const Vector3& v)
    {
        _x -= v._x; _y -= v._y; _z -= v._z; return *this;
    }
    
    //! Component-wise scalar subtraction assignment operator.
    const Vector3 & operator-=(float a) {_x -= a; _y -= a; _z -= a; return *this;}


    //! Scalar multiplication operator.
    Vector3 operator*(float a) const {return Vector3(_x * a, _y * a, _z * a);}
    
    //! Component-wise vector multiplication operator.
    Vector3 operator*(const Vector3& v) const
    {
        return Vector3(_x * v._x, _y * v._y, _z * v._z);
    }
    
    //! Scalar multiplication-assignment operator.
    const Vector3 & operator*=(float a) {_x *= a; _y *= a; _z *= a; return *this;}
    
    //! Component-wise vector multiplication-assignment operator.
    const Vector3 & operator*=(const Vector3& v)
    {
        _x *= v._x; _y *= v._y; _z *= v._z; return *this;
    }
    
    //! Negation operator.
    Vector3 operator-() const {return Vector3(-_x, -_y, -_z);}
    const Vector3 & negate() {_x = -_x; _y = -_y; _z = -_z; return *this;}


    //! Scalar division operator.
    Vector3 operator/(float a) const
    {
        float inv = float(1) / a;
        return Vector3(_x * inv, _y * inv, _z * inv);
    }
    
    //! Component-wise vector division operator.
    Vector3 operator/(const Vector3 & v) const
    {
        return Vector3(_x / v._x, _y / v._y, _z / v._z);
    }
    
    //! Scalar division-assignment operator.
    const Vector3 & operator/=(float a)
    {
        float inv = float(1) / a;
        _x *= inv; _y *= inv; _z *= inv;
        return *this;
    }
    
    //! Component-wise vector division-assignment operator.
    const Vector3 & operator/=(const Vector3 & v)
    {
        _x /= v._x; _y /= v._y; _z /= v._z; return *this;
    }


    //! Vector equivalence operator.
    /*!
        Tests to see if each component of \a v is equal to each component of
        this Vector3.
    */
    bool operator==(const Vector3 & v) const
    {
        return(v._x == _x && v._y == _y && v._z == _z);
    }
    
    //! Vector difference operator.
    /*!
        Tests to see if an_y component is different between the two Vec3s.
    */
    bool operator!=(const Vector3 & v) const
    {
        return(v._x != _x || v._y != _y || v._z != _z);
    }


    //! Length<sup>2</sup>.
    /*!
        Returns the geometric length<sup>2</sup> of the vector.
    */
    float length2() const;
    
    //! Length.
    /*!
        Returns the geometric length of the vector.
    */
    float length() const {return sqrtf(length2());}
    
    //! Normalizes the vector and return its length.
    /*!
        Scales each component of the vector in order to get unit
        length without changing direction.
    
        \return The length of the vector prior to normalization.
    */
    float unitize()
    {
        float l = length();
        *this /= l;
        return l;
    }
    
    //! Normalize a vector and return a reference to it.
    /*!
        Scales each component of the vector in order to get unit
        length without changing direction.
    
        \return A reference to the vector.
    */
    const Vector3 & normalize()
    {
        return (*this /= length());
    }
    
    //! Return a normalized copy of the vector.
    Vector3 normalized() const
    {
        return( *this / length());
    }
    
    //! Return a rotated copy of the vector
    Vector3 rotated(float theta, const Vector3 & w) const;
    
    //! Rotate this vector about another vector, w, by theta radians.
    const Vector3 & rotate(float theta, const Vector3 & w)
    {
	return *this = rotated(theta, w);
    }
};


//! Multiply a scalar by a Vec3.
inline Vector3
operator*(float s, const Vector3& v)
{
    return Vector3(v._x * s, v._y * s, v._z * s);
}


//! The dot product of two Vec3s.
inline float 
dot(const Vector3 & a, const Vector3 & b)
{
    return a._x * b._x + a._y * b._y + a._z * b._z;
}


//! The cross product of two Vec3s.
inline Vector3
cross(const Vector3 & a, const Vector3 & b)
{
    return Vector3(a._y * b._z - a._z * b._y,
                   a._z * b._x - a._x * b._z,
                   a._x * b._y - a._y * b._x);
}


inline float
Vector3::length2() const
{
    return dot(*this, *this);
}


//! Return a rotated copy of the vector
inline Vector3
Vector3::rotated(float theta, const Vector3 & w) const
{
    float c = cosf(theta);
    float s = sinf(theta);

    Vector3 v0 = dot(*this, w) * w;
    Vector3 v1 = *this - v0;
    Vector3 v2 = cross(w, v1);

    return v0 + c * v1 + s * v2;
}


inline std::ostream &
operator<<(std::ostream& out, const Vector3& v)
{
    return out << v._x << " " << v._y << " " << v._z ;
}
#undef Vector3

#endif // CSE168_VECTOR3_H_INCLUDED
