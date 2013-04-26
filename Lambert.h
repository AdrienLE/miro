#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"

class Lambert : public DiffuseMaterial
{
public:
    Lambert(const Vector3 & color = Vector3(1));
	Lambert(Material* kd);
    virtual ~Lambert();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
protected:
    Material* m_kd;
};

#endif // CSE168_LAMBERT_H_INCLUDED
