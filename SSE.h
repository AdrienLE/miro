#pragma once

#include <math.h>
#include <smmintrin.h>

#if defined(WIN32) && defined(SSE)
#define WSSEALIGN __declspec( align( 16 ) )
#else
#define WSSEALIGN
#endif

// TODO later
#if defined(__GNUC__) && defined(SSE)
#define GSSEALIGN __attribute__((aligned(16)))
#else
#define GSSEALIGN
#endif

inline float _mm_extract_ps1(__m128 a, int i)
{
	union
	{
		__m128 m;
		float f[4];
	} u;
	u.m = a;
	return u.f[i];
}

inline int _mm_extract_int(__m128 a, int i)
{
	union
	{
		__m128 m;
		int f[4];
	} u;
	u.m = a;
	return u.f[i];
}

class SSE
{
public:
    SSE();

    bool hasSSE() const { return m_hasSSE; }

private:
    bool m_hasSSE;
};

extern SSE sse_info;
