#include "Ambient.h"

Ambient::~Ambient() {}

Vector3 Ambient::shade(const Ray &, const HitInfo &, const Scene &) const
{
	return m_a;
}
