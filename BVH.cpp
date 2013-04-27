#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "Sphere.h"
#include "Triangle.h"

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_objects = objs;

    m_categories_objects.resize(NB_OBJS);
    for (size_t i = 0; i < m_objects->size(); ++i)
    {
        m_categories_objects[(*m_objects)[i]->type()].plain.push_back((*m_objects)[i]->ptr());
    }
    m_categories_objects[TRIANGLE].sse_preprocessed = Triangle::preProcess(m_categories_objects[TRIANGLE].plain);
    m_intersect_fcts.resize(NB_OBJS);
    m_intersect_fcts[TRIANGLE] = &Triangle::doIntersect;
    m_intersect_fcts[SPHERE] = &Sphere::doIntersect;
}


bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;

    for (size_t i = 0; i < NB_OBJS; ++i)
    {
        if (m_intersect_fcts[i] && m_intersect_fcts[i](m_categories_objects[i], tempMinHit, ray, tMin, tMax) && tempMinHit.t < minHit.t)
        {
            minHit = tempMinHit;
            hit = true;
        }
    }
    
    return hit;
}
