#pragma once

#include "Material.h"
#include <vector>

class LinearCombination : public Material
{
public:
	LinearCombination(std::vector<std::pair<float, Material *> > const &v) : m_combination(v) {}
	LinearCombination() {}

	void addMember(float a, Material *m)
	{
		m_combination.push_back(std::make_pair(a, m));
	}

	void assertValid() const;

	virtual ~LinearCombination();

	virtual void preCalc() {}
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;

protected:
	std::vector<std::pair<float, Material *> > m_combination;
};
