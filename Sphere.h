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
    virtual bool intersectAll(std::vector<Object *> const &objects, HitInfo& result, const Ray& ray,
                              float tMin = 0.0f, float tMax = MIRO_TMAX);

    virtual ObjType type() const;

protected:
    Vector3 m_center;
    float m_radius;
    float m_radius2;

    static bool Sphere::doIntersect(std::vector<Object *> const &objects, HitInfo& result, const Ray& ray,
                                    float tMin, float tMax);
};

#endif // CSE168_SPHERE_H_INCLUDED
