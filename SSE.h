#pragma once

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
#define GSSEALIGN 