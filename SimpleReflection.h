#pragma once

#include "Material.h"

class SimpleReflection : public SpecularMaterial
{
public:
	SimpleReflection();
	virtual ~SimpleReflection();

	virtual void preCalc() {}
	virtual Vector3 shade(const Ray &ray, const HitInfo &hit,
						  const Scene &scene) const;
};
