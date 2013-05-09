#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Box.h"
#include "BoundingBox.h"
#include <limits>

#define INF std::numeric_limits<float>::infinity()

#define CBOX 1.f
#define CTRI 1.f
#define SIZE_LEAF 8

static float costNTris(int n)
{
    return ((n / 4) + (n % 4 != 0)) * CTRI;
}

static float cost(float areaA, int objA, float areaB, int objB, float areaCinv)
{
    return 2 * CBOX + (areaA * costNTris(objA) + areaB * costNTris(objB)) * areaCinv;
}

void BVH::recBuildBBox(Objects *objs, BVH::BBoxNode *prev_node)
{
    if (objs->size() <= SIZE_LEAF)
    {
        prev_node->objs = new std::vector<IntersectObjects>();
        prev_node->objs->resize(NB_OBJS);
        for (size_t i = 0; i < objs->size(); ++i)
        {
            (*prev_node->objs)[(*objs)[i]->type()].plain.push_back((*objs)[i]->ptr());
        }
        (*prev_node->objs)[TRIANGLE].sse_preprocessed = Triangle::preProcess((*prev_node->objs)[TRIANGLE].plain);
        return;
    }
    Objects a, b;
    BoundingBox abox(Vector3(-INF, -INF, -INF), Vector3(INF, INF, INF));
    BoundingBox bbox = abox;
    float areaA = 0.f, areaB = 0.f;
    float areaCinv = 1.f / prev_node->box.area();
    for (Objects::const_iterator it = objs->begin(); it != objs->end(); ++it)
    {
        Vector3 min = (*it)->minVector(), max = (*it)->maxVector();
        BoundingBox anew = abox;
        anew.setA(abox.getA().max(max));
        anew.setB(abox.getB().min(min));
        float areaNewA = anew.area();
        BoundingBox bnew = bbox;
        bnew.setA(bbox.getA().max(max));
        bnew.setB(bbox.getB().min(min));
        float areaNewB = bnew.area();
        float costa = cost(areaNewA, a.size() + 1, areaB, b.size(), areaCinv);
        float costb = cost(areaA, a.size(), areaNewB, b.size() + 1, areaCinv);
        if (costa < costb)
        {
            abox = anew;
            areaA = areaNewA;
            a.push_back(*it);
        }
        else
        {
            bbox = bnew;
            areaB = areaNewB;
            b.push_back(*it);
        }
    }
    prev_node->a = new BBoxNode;
    prev_node->a->box = abox;
    prev_node->b = new BBoxNode;
    prev_node->b->box = bbox;
    recBuildBBox(&a, prev_node->a);
    recBuildBBox(&b, prev_node->b);
}

BoundingBox BVH::objectBox(Objects *objs)
{
    Vector3 min = Vector3(INF, INF, INF);
    Vector3 max = Vector3(-INF, -INF, -INF);
    for (Objects::const_iterator it = objs->begin(); it != objs->end(); ++it)
    {
        min = min.min((*it)->minVector());
        max = max.max((*it)->maxVector());
    }
    return BoundingBox(min, max);
}

void BVH::printHierarchy(BBoxNode *node, int ind)
{
    printf("%f\n", node->box.area());
    if (node->a && node->b)
    {
        printHierarchy(node->a, ind + 1);
        printHierarchy(node->b, ind + 1);
    }
    else
        printf("%f\n", (*node->objs)[TRIANGLE].plain.size());
}

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_root = new BBoxNode;
    m_root->box = objectBox(objs);
    recBuildBBox(objs, m_root);
    //printHierarchy(m_root, 0);

    m_objects = objs;

    // OBSOLETE: start
    m_categories_objects.resize(NB_OBJS);
    for (size_t i = 0; i < m_objects->size(); ++i)
    {
        m_categories_objects[(*m_objects)[i]->type()].plain.push_back((*m_objects)[i]->ptr());
    }
    m_categories_objects[TRIANGLE].sse_preprocessed = Triangle::preProcess(m_categories_objects[TRIANGLE].plain);
    // OBSOLETE: stop

    m_intersect_fcts.resize(NB_OBJS);
    m_intersect_fcts[TRIANGLE] = &Triangle::doIntersect;
    m_intersect_fcts[SPHERE] = &Sphere::doIntersect;
    m_intersect_fcts[BOX] = &Box::doIntersect;
}

bool
BVH::rec_intersect(BBoxNode *node, HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	bool hit = false;
	HitInfo tempMinHit;
	minHit.t = MIRO_TMAX;

	if (node->a == NULL && node->b == NULL)
	{
		for (size_t i = 0; i < NB_OBJS; ++i)
		{
			if (m_intersect_fcts[i] && m_intersect_fcts[i]((*node->objs)[i], tempMinHit, ray, tMin, tMax) && tempMinHit.t < minHit.t)
			{
				minHit = tempMinHit;
				hit = true;
			}
		}

	}
	else
	{
		if (node->a && node->a->box.doIntersect(tempMinHit, ray, tMin, tMax))
		{
			hit = rec_intersect(node->a, minHit, ray, tMin, tMax);
		}
		if (node->b && node->b->box.doIntersect(tempMinHit, ray, tMin, tMax))
		{
			bool tmpHit = rec_intersect(node->b, tempMinHit, ray, tMin, tMax);
			if (tmpHit && (!hit || (hit && tempMinHit.t < minHit.t)))
			{
				minHit = tempMinHit;
				hit = true;
			}
		}
	}
	return hit;
}

bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;

	return rec_intersect(m_root, minHit, ray, tMin, tMax);
}
