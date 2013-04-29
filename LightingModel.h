#pragma once
#include "material.h"
class LightingModel :
    public Material
{
public:
    LightingModel(void);
    virtual ~LightingModel(void);



    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
        const Scene& scene) const;

    

private:
    void assertCoefficients() const;

    TextureMaterial *m_tex;
    float m_difCoef;
    DiffuseMaterial *m_dif;
    float m_reflCoef;
    ReflectMaterial *m_refl;
    float m_refrCoef;
    RefractMaterial *m_refr;
    HighlightsMaterial *m_high;
};

