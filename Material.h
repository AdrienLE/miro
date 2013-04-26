#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"

class Material
{
public:
    Material(const Vector3& color = Vector3(1.0f, 1.0f, 1.0f));
    virtual ~Material();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;

protected:
	Vector3 m_color;
};

// The distinction is being made because specular materials
// will typically be wrappers around a diffuse material
class DiffuseMaterial : public Material {};
class SpecularMaterial : public Material {};

#endif // CSE168_MATERIAL_H_INCLUDED
