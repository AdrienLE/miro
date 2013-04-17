#ifndef CSE168_OBJECT_H_INCLUDED
#define CSE168_OBJECT_H_INCLUDED

#include <vector>
#include "Miro.h"
#include "Material.h"

enum ObjType
{
    SPHERE,
    TRIANGLE,
    BLPATCH,
    NB_OBJS
};

class Object
{
public:
    Object() {}
    virtual ~Object() {}

    void setMaterial(const Material* m) {m_material = m;}

    virtual void renderGL() {}
    virtual void preCalc() {}

    // Intersects a bunch of objects of the same type
    virtual bool intersectAll(std::vector<Object *> const &objects, HitInfo& result, const Ray& ray,
                              float tMin = 0.0f, float tMax = MIRO_TMAX) = 0;

    virtual ObjType type() const = 0;

protected:
    const Material* m_material;
};

typedef std::vector<Object*> Objects;

#endif // CSE168_OBJECT_H_INCLUDED
