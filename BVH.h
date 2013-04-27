#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "IntersectObjects.h"

class BVH
{
public:
    void build(Objects * objs);

    bool intersect(HitInfo& result, const Ray& ray,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;

protected:
    Objects * m_objects;
    std::vector<IntersectObjects> m_categories_objects;
    typedef bool (*Intersect)(IntersectObjects const &objects, HitInfo& result,
                              const Ray& ray, float tMin, float tMax);
    std::vector<Intersect> m_intersect_fcts;
};

#endif // CSE168_BVH_H_INCLUDED
