#pragma once

//#define SSE

#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include "SSE.h"
#include "MyBoost.h"
#include "SSEObject.h"

// A template vector class that handles both 3D and 4D vectors

#ifdef WIN32
#pragma warning(disable:4305) // disable useless warnings
#pragma warning(disable:4244)
#pragma warning(disable:4200) // Don't warn about zero-size arrays
#endif

// It would be best to have an SSE based sincos, but it's actually pretty complicated.
#if defined(__GNUC__)
#define SINCOS(x, s, c) sincosf(x, s, c)
#else
#define SINCOS(x, s, c) do { *s = sinf(x); *c = cosf(x); } while (0)
#endif

#ifdef SSE
// Xoring this with a vector negates that vector
static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
#endif

template<int D>
class TVector : public SSEObject
{
    BOOST_STATIC_ASSERT_MSG(D == 3 || D == 4, "Can only create 3D or 4D vectors");

public:
#ifdef SSE
    __m128 mm;
#else
    union
    {
        float elems[D];
        struct
        {
            float x;
            float y;
            float z;
            float w;
            //typename member_if<D == 4, float>::type w;
        };
    };
#endif

#if !defined(SSE)
    TVector()
    {
        for (int i = 0; i < D; ++i)
            elems[i] = 0;
    }

    TVector(float s)
    {
        for (int i = 0; i < 3; ++i)
            elems[i] = s;
        if (D == 4)
            elems[3] = 1;
    }

    TVector(float x, float y, float z): x(x), y(y), z(z)
    {
        if (D == 4)
            elems[3] = 1;
    }

    TVector(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {BOOST_STATIC_ASSERT(D==4);}

    TVector(TVector<D - 1> const &o): x(o.x), y(o.y), z(o.z), w(1) {BOOST_STATIC_ASSERT(D==4);}

    TVector<D> &operator=(const TVector<D> &a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] = a.elems[i];
        return *this;
    }

    TVector<D> &operator=(float a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] = a;
        return *this;
    }

    void set(float a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] = a;
    }

    void set(float a, float b, float c) { x = a; y = b; z = c; }
    void set(float a, float b, float c, float d) { BOOST_STATIC_ASSERT(D==4); x = a; y = b; z = c; w = d; }
    void set(TVector<D> const &a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] = a.elems[i];
    }

    float &operator[](int i) { return elems[i]; }
    float const & operator[](int i) const { return elems[i]; }

    TVector<D> operator+(const TVector<D> &a) const
    {
        TVector<D> n(*this);
        n += a;
        return n;
    }

    TVector<D>& operator+=(float s)
    {
        for (int i = 0; i < D; ++i)
            elems[i] += s;
        return *this;
    }

    TVector<D>& operator+=(TVector<D> const &a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] += a.elems[i];
        return *this;
    }

    TVector<D> operator-(const TVector<D> &a) const
    {
        TVector<D> n(*this);
        n -= a;
        return n;
    }

    TVector<D>& operator-=(float a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] -= a;
        return *this;
    }

    TVector<D>& operator-=(TVector<D> const &a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] -= a.elems[i];
        return *this;
    }

    TVector<D> operator*(const TVector<D> &a) const 
    {
        TVector<D> n(*this);
        n *= a;
        return n;
    }

    TVector<D> operator*(float a)
    {
        TVector<D> n(*this);
        n *= a;
        return n;
    }

    TVector<D>& operator*=(float a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] *= a;
        return *this;
    }


    TVector<D>& operator*=(TVector<D> const &a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] *= a.elems[i];
        return *this;
    }

    TVector<D> operator-() const
    {
        TVector<D> n(*this);
        n.negate();
        return n;
    }
    TVector &negate()
    {
        for (int i = 0; i < D; ++i)
            elems[i] = -elems[i];
        return *this;
    }


    TVector<D> operator/(float a) const
    {
        return *this * (1/a);
    }

    TVector<D> operator/(const TVector<D> &a) const
    {
        TVector<D> n(*this);
        n /= a;
        return n;
    }

    TVector<D>& operator/=(float a)
    {
        a = 1/a;
        for (int i = 0; i < D; ++i)
            elems[i] *= a;
        return *this;
    }

    TVector<D>& operator/=(TVector<D> const &a)
    {
        for (int i = 0; i < D; ++i)
            elems[i] /= a.elems[i];
        return *this;
    }

    bool operator==(const TVector<D> &v) const
    {
        for (int i == 0; i < D; ++i)
            if (elems[i] != v.elems[i])
                return false;
        return true;
    }

    bool operator!=(const TVector<D> &v) const
    {
        for (int i == 0; i < D; ++i)
            if (elems[i] == v.elems[i])
                return false;
        return true;
    }

    float length2() const
    {
        return dot(*this);
    }

    float length() const {return sqrtf(length2());}
    float rlength() const { return 1/length(); }

    float unitize()
    {
        float l = length();
        *this /= l;
        return l;
    }

    TVector<D>& normalize()
    {
        return (*this /= length());
    }

    TVector<D> normalized() const
    {
        return *this / length();
    }

    TVector<D> rotated(float theta, const TVector<D> &w) const
    {
        float c;
        float s;
        SINCOS(theta, &s, &c);

        Vector3 v0 = dot(w) * w;
        Vector3 v1 = *this - v0;
        Vector3 v2 = w.cross(v1);

        return v0 + c * v1 + s * v2;
    }

    TVector<D> &rotate(float theta, const TVector<D> &w)
    {
        return *this = rotated(theta, w);
    }

    float dot(const TVector<D> &w) const
    {
        float res = 0;
        for (int i = 0; i < D; ++i)
            res += elems[i] * w.elems[i];
        return res;
    }

    typename boost::enable_if_c<D == 3, TVector<D> >::type cross(const TVector<D> &w) const
    {
        return TVector<D>(y * w.z - z * w.y,
                          z * w.x - x * w.z,
                          x * w.y - y * w.x);
    }
#else

// This is largely based on: http://fastcpp.blogspot.com/2011/12/simple-vector3-class-with-sse-support.html

    TVector(): mm(_mm_setzero_ps()) {}
    TVector(float x, float y, float z): mm(_mm_set_ps(D == 4, z, y, x)) {}
    TVector(float s): mm(_mm_set_ps(D == 4, s, s, s)) {}
    TVector(float x, float y, float z, float w): mm(_mm_set_ps(w, z, y, x)) {BOOST_STATIC_ASSERT(D == 4);}
    TVector(TVector<D - 1> const &w): mm(_mm_set_ps(1, w.z, w.y, w.x)) {BOOST_STATIC_ASSERT(D == 4);}
    TVector(__m128 m): mm(m) {}

    // Please don't access individual elements, cuz it's bad mckay?
    float w() const { float r; _mm_store_ss(&r, _mm_shuffle_ps(mm, mm, _MM_SHUFFLE(3, 3, 3, 3))); return r; }
    float z() const { float r; _mm_store_ss(&r, _mm_shuffle_ps(mm, mm, _MM_SHUFFLE(2, 2, 2, 2))); return r; }
    float y() const { float r; _mm_store_ss(&r, _mm_shuffle_ps(mm, mm, _MM_SHUFFLE(1, 1, 1, 1))); return r; }
    float x() const { float r; _mm_store_ss(&r, _mm_shuffle_ps(mm, mm, _MM_SHUFFLE(0, 0, 0, 0))); return r; }

    void set(float a) { mm = (D == 4? _mm_set_ps1(a): _mm_set_ps(0, a, a, a)); }
    void set(float a, float b, float c) { mm = _mm_set_ps(D == 4, c, b, a); }
    void set(const TVector<D> &v) { mm = v.mm; }
    void set(float a, float b, float c, float d) { BOOST_STATIC_ASSERT(D == 4); mm = _mm_set_ps(d, c, b, a); }

    TVector<D> &operator=(const TVector<D> &b) { mm = b.mm; return *this; }
    TVector<D> &operator=(float b) { mm = _mm_set_ps(D == 4, z, y, x); }

    TVector<D> operator+(const TVector<D> &b) const { return _mm_add_ps(mm, b.mm); }
    TVector<D> operator-(const TVector<D> &b) const { return _mm_sub_ps(mm, b.mm); }
    TVector<D> operator*(const TVector<D> &b) const { return _mm_mul_ps(mm, b.mm); }
    TVector<D> operator/(const TVector<D> &b) const { return _mm_div_ps(mm, b.mm); }

    TVector<D> &operator+=(const TVector<D> &b) { mm = _mm_add_ps(mm, b.mm); return *this; }
    TVector<D> &operator-=(const TVector<D> &b) { mm = _mm_sub_ps(mm, b.mm); return *this; }
    TVector<D> &operator*=(const TVector<D> &b) { mm = _mm_mul_ps(mm, b.mm); return *this; }
    TVector<D> &operator/=(const TVector<D> &b) { mm = _mm_div_ps(mm, b.mm); return *this; }

    TVector<D> operator-() const { return _mm_xor_ps(mm, SIGNMASK); }
    TVector<D> &negate() { mm = _mm_xor_ps(mm, SIGNMASK); return *this; }

#define _MM_SET_PS1_ADD(b) _mm_set_ps1(b)

    TVector<D> operator+(float b) const { return _mm_add_ps(mm, _MM_SET_PS1_ADD(b)); }
    TVector<D> operator-(float b) const { return _mm_sub_ps(mm, _MM_SET_PS1_ADD(b)); }
    TVector<D> operator*(float b) const { return _mm_mul_ps(mm, _mm_set_ps1(b)); }
    TVector<D> operator/(float b) const { return _mm_div_ps(mm, _mm_set_ps1(b)); }

    TVector<D> &operator+=(float b) { mm = _mm_add_ps(mm, _MM_SET_PS1_ADD(b)); return *this; }
    TVector<D> &operator-=(float b) { mm = _mm_sub_ps(mm, _MM_SET_PS1_ADD(b)); return *this; }
    TVector<D> &operator*=(float b) { mm = _mm_mul_ps(mm, _mm_set_ps1(b)); return *this; }
    TVector<D> &operator/=(float b) { mm = _mm_div_ps(mm, _mm_set_ps1(b)); return *this; }


#define PCROSS(mm, b) (\
        _mm_sub_ps( \
            _mm_mul_ps(_mm_shuffle_ps(mm, mm, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))), \
            _mm_mul_ps(_mm_shuffle_ps(mm, mm, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))) \
        ))

    TVector<D> cross(const TVector<D> &b) const
    {
        BOOST_STATIC_ASSERT(D == 3);
        return PCROSS(mm, b.mm);
    }

#define DOTPRODUCT(r, a, b) do { r = _mm_dp_ps(a, b, D == 3 ? 0x71: 0xf1); } while (0)

    // Some help from this: http://stackoverflow.com/questions/4120681/how-to-calculate-vector-dot-product-using-sse-intrinsic-functions-in-c
    float dot(const TVector<D> &b) const
    {
        __m128 r;
        DOTPRODUCT(r, mm, b.mm);
        return _mm_cvtss_f32(r);
    }

    float length2() const { __m128 r; DOTPRODUCT(r, mm, mm); return _mm_cvtss_f32(r); }
    float length() const { __m128 r; DOTPRODUCT(r, mm, mm); r = _mm_sqrt_ss(r); return _mm_cvtss_f32(r); }
    float rlength() const { __m128 r; DOTPRODUCT(r, mm, mm); r = _mm_rsqrt_ss(r); return _mm_cvtss_f32(r); }

    // TODO: there is some redundancy here, maybe simplify all this (while keeping performance in mind though)

    // Needs to be tested
    float unitize()
    {
        __m128 r;
        r = _mm_dp_ps(mm, mm, D == 4? 0xff: 0x7f);
        r = _mm_sqrt_ps(r);
        mm = _mm_div_ps(mm, r);
        return _mm_cvtss_f32(r);
    }
    // This should be faster than unitize because it uses a multiplication instead of a division
    TVector<D> &normalize()
    {
        __m128 r;
        r = _mm_dp_ps(mm, mm, D == 4? 0xff: 0x7f);
        r = _mm_rsqrt_ps(r);
        mm = _mm_mul_ps(mm, r);
        return *this;
    }
    TVector<D> normalized() const
    {
        __m128 r;
        r = _mm_dp_ps(mm, mm, D == 4? 0xff: 0x7f);
        r = _mm_rsqrt_ps(r);
        return _mm_mul_ps(mm, r);
    }

    // To test
    TVector<D> rotated(float theta, const TVector<D> &w) const
    {
        BOOST_STATIC_ASSERT(D == 3);
        float si, co;
        SINCOS(theta, &si, &co);
        __m128 c = _mm_set_ps1(co);
        __m128 s = _mm_set_ps1(si);

        __m128 r;
        DOTPRODUCT(r, mm, w.mm);
        __m128 v0 = _mm_mul_ps(w.mm, r);

        __m128 v1 = _mm_sub_ps(mm, v0);

        __m128 v2 = PCROSS(w.mm, v1);

        return _mm_add_ps(v0, _mm_add_ps(_mm_mul_ps(c, v1), _mm_mul_ps(s, v2)));
    }

    TVector<D> &rotate(float theta, TVector<D> const &b)
    {
        return (*this = rotated(theta, b));
    }
#endif
};

template<>
class TVector<2>
{
private:
    TVector();
    ~TVector();
    TVector(TVector<2> const &);
    TVector &operator=(TVector<2> const &);
};

template<int D>
inline TVector<D>
operator*(float s, const TVector<D>& v)
{
    return v * s;
}

template<int D>
inline float dot(const TVector<D> &a, const TVector<D> &b) { return a.dot(b); }
template<int D>
inline TVector<D> cross(const TVector<D> &a, const TVector<D> &b) { return a.cross(b); }

template<int D>
inline std::ostream & operator<<(std::ostream& out, const TVector<D> &v)
{
    for (int i = 0; i < D; ++i) {
        out << v[i];
        if (D - 1 != i)
            out << ' ';
    }
    return out;
}