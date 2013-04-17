#ifndef CSE168_TRIANGLE_H_INCLUDED
#define CSE168_TRIANGLE_H_INCLUDED

#include "Object.h"

/*
    The Triangle class stores a pointer to a mesh and an index into its
    triangle array. The mesh stores all data needed by this Triangle.
*/
class Triangle : public Object, public SSEObject
{
public:
    Triangle(TriangleMesh * m = 0, unsigned int i = 0);
    virtual ~Triangle();

    void setIndex(unsigned int i) {m_index = i;}
    void setMesh(TriangleMesh* m) {m_mesh = m;}

    virtual void renderGL();
    virtual bool intersectAll(std::vector<Object *> const &objects, HitInfo& result, const Ray& ray,
                              float tMin = 0.0f, float tMax = MIRO_TMAX);

    virtual ObjType type() const;
    
protected:
    TriangleMesh* m_mesh;
    unsigned int m_index;
};

#endif // CSE168_TRIANGLE_H_INCLUDED
