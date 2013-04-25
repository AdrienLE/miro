#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"

class Lambert : public DiffuseMaterial
{
public:
    Lambert(const Vector3 & kd = Vector3(1));
    virtual ~Lambert();

    const Vector3 & kd() const {return m_kd;}

    void setKd(const Vector3 & kd) {m_kd = kd;}

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
protected:
    Vector3 m_kd;
};

#endif // CSE168_LAMBERT_H_INCLUDED
