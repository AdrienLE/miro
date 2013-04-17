#include "BLPatch.h"
#include "Ray.h"
#include "Console.h"

BLPatch::BLPatch()
{
}

BLPatch::~BLPatch()
{
}

void
BLPatch::renderGL()
{

}

bool
BLPatch::intersectAll(std::vector<Object *> const &objects, HitInfo& result, const Ray& ray,
                   float tMin, float tMax)
{
    return false;
}

ObjType BLPatch::type() const { return BLPATCH; }