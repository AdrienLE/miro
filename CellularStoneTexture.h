#ifndef CSE168_CELLULAR_STONE_TEXTURE_H_INCLUDED
#define CSE168_CELLULAR_STONE_TEXTURE_H_INCLUDED

#include "Material.h"

class CellularStoneTexture : public DiffuseMaterial
{
public:
    CellularStoneTexture(const Vector3 & color = Vector3(1));
    virtual ~CellularStoneTexture();

    virtual void preCalc();

    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;

protected:
	Vector3 getTextureColorAt(const Vector3& pos) const;

protected:
	Vector3	m_color;
};

#endif // CSE168_CELLULAR_STONE_TEXTURE_H_INCLUDED
