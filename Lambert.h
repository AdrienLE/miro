#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"

class Lambert : public DiffuseMaterial
{
public:
    Lambert(const Vector3 & color = Vector3(1), const Vector3 &ambient = Vector3(0));
	Lambert(shared_ptr<Material> kd, const Vector3 &ambient = Vector3(0));
    virtual ~Lambert();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
protected:
    shared_ptr<Material> m_kd;
    Vector3 m_ka;
};

#endif // CSE168_LAMBERT_H_INCLUDED
