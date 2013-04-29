#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"

class Phong : public Material
{
public:
    Phong(const Vector3 & diffuse, const Vector3 &specular, const Vector3 &ambient);
	Phong(shared_ptr<Material> kd, shared_ptr<Material> ks, const Vector3 &ambient);
	Phong(shared_ptr<Material> kd, const Vector3 &ambient = Vector3(0));
    virtual ~Phong();

    void setDiffuseProportion(float f) {m_dp = f;}
    void setSpecularProportion(float f) {m_sp = f;}
    void setPhong(float f) {m_phong = f;}
    void setRefraction(float indice, float proportion) {m_refr = indice; m_tp = proportion;}

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
protected:
    shared_ptr<Material> m_kd;
    shared_ptr<Material> m_ks;
    Vector3 m_ka;
    float m_dp;
    float m_sp;
    float m_tp;
    float m_phong;
    float m_refr;
};

#endif // CSE168_LAMBERT_H_INCLUDED
