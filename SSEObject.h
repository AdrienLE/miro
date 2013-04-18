#pragma once

#include <stdlib.h>
#include "SSE.h"


#ifndef WIN32
inline void *_aligned_malloc(size_t x, size_t align)
{
  void *p;
  posix_memalign(&p, align, x);
  return p;
}
#define _aligned_free free
#endif

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
