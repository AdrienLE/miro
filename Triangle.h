#ifndef CSE168_TRIANGLE_H_INCLUDED
#define CSE168_TRIANGLE_H_INCLUDED

#include "Object.h"

/*
    The Triangle class stores a pointer to a mesh and an index into its
    triangle array. The mesh stores all data needed by this Triangle.
*/
class Triangle : public Object
{
public:
    Triangle(TriangleMesh * m = 0, unsigned int i = 0);
    virtual ~Triangle();

    void setIndex(unsigned int i) {m_index = i;}
    void setMesh(TriangleMesh* m) {m_mesh = m;}

    virtual void renderGL();

    static bool doIntersect(std::vector<void *> const &, HitInfo &, const Ray &, float, float);

    virtual ObjType type() const { return TRIANGLE; }
    virtual void *ptr() { return this; }

protected:
    TriangleMesh* m_mesh;
    unsigned int m_index;
};

#endif // CSE168_TRIANGLE_H_INCLUDED
