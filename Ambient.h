#pragma once

#include "Material.h"
#include "Scene.h"

class Ambient : public Material
{
public:
	Ambient(): m_a(Vector3(0)) {}
	Ambient(Vector3 const &v): m_a(v) {}
	virtual ~Ambient();

	Vector3 const &getA() const { return m_a; }
	void setA(Vector3 const &a) { m_a = a; }

	virtual void preCalc() {}

	virtual Vector3 shade(const Ray &, const HitInfo &, const Scene &) const;

protected:
	Vector3 m_a;
};