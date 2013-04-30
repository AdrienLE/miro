#include "Material.h"

Material::Material(const Vector3& color) : m_color(color), m_castShadow(true)
{
}

Material::~Material()
{
}

Vector3
Material::shade(const Ray&, const HitInfo&, const Scene&) const
{
    return m_color;
}
