#pragma once

#include <vector>
#include "SSEObject.h"

struct IntersectObjects
{
    IntersectObjects() : sse_preprocessed(0) {}

    SSEObject *sse_preprocessed;
    std::vector<void *> plain;
};
