#pragma once

#include <math.h>
#include <smmintrin.h>

#ifdef SSE4
#define SSE
#endif

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
