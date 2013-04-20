#ifndef CSE168_SPHERE_H_INCLUDED
#define CSE168_SPHERE_H_INCLUDED

#include "Vector3.h"
#include "Object.h"

class Sphere : public Object, public SSEObject
{
public:
    Sphere();
    virtual ~Sphere();

    void setCenter(const Vector3& v)    {m_center = v;}
    void setRadius(const float f)       {m_radius = f; m_radius2 = m_radius * m_radius;}

    const Vector3& center() const       {return m_center;}
    float radius() const                {return m_radius;}

    virtual void renderGL();

    static bool doIntersect(std::vector<void *> const &objects, HitInfo& result,
                            const Ray& ray, float tMin, float tMax);

    virtual ObjType type() const;

    virtual void *selfPointer();

protected:
    Vector3 m_center;
    float m_radius2;
    float m_radius;
};

#endif // CSE168_SPHERE_H_INCLUDED
