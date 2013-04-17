#pragma once

#include "SSE.h"

class WSSEALIGN SSEObject
{
public:
#ifdef SSE
    void *operator new[](size_t x) { return _aligned_malloc(x, 16); }
    void operator delete[](void *x) { if (x) return _aligned_free(x); }
    void *operator new(size_t x) { return _aligned_malloc(x, 16); }
    void operator delete(void *x) { if (x) return _aligned_free(x); }
#endif
} GSSEALIGN;
