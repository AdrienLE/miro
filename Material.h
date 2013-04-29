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
class TextureMaterial : public Material {};
class DiffuseMaterial : public Material
{
public:
    void setTexture(shared_ptr<TextureMaterial> tex) { m_tex = tex; }
protected:
    shared_ptr<TextureMaterial> m_tex;
};
class SpecularMaterial : public Material {};
class ReflectMaterial : public SpecularMaterial {};
class RefractMaterial : public SpecularMaterial {};
class HighlightsMaterial : public SpecularMaterialMaterial
{
public:
    void setTexture(shared_ptr<TextureMaterial> tex) { m_tex = tex; }
protected:
    shared_ptr<TextureMaterial> m_tex;
};

#endif // CSE168_MATERIAL_H_INCLUDED
