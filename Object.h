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

    // Needs a static function that intersects a bunch of objects of the same type

    virtual ObjType type() const = 0;

    // This returns a pointer to yourself that can be cast directly into the class:
    // casting from, say, Object to Sphere involves some computation, so we want to avoid
    // that.
    virtual void *selfPointer() = 0;

protected:
    const Material* m_material;
};

typedef std::vector<Object*> Objects;

#endif // CSE168_OBJECT_H_INCLUDED
