#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"

class BVH
{
public:
    void build(Objects * objs);

    bool intersect(HitInfo& result, const Ray& ray,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;

protected:
    Objects * m_objects;
    std::vector<std::vector<void *> > m_categories_objects;
    typedef bool (*Intersect)(std::vector<void *> const &objects, HitInfo& result,
                              const Ray& ray, float tMin, float tMax);
    std::vector<Intersect> m_fcts;
};

#endif // CSE168_BVH_H_INCLUDED
