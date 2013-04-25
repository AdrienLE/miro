#include "LinearCombination.h"
#include "Scene.h"
#include <assert.h>

LinearCombination::~LinearCombination() {}

void LinearCombination::assertValid() const
{
	float total = 0;
	for (int i = 0; i < m_combination.size(); ++i)
		total += m_combination[i].first;
	assert(total >= 0 && total <= 1);
}

Vector3 LinearCombination::shade(const Ray& ray, const HitInfo& hit,
                                 const Scene& scene) const
{
	Vector3 result;
	for (int i = 0; i < m_combination.size(); ++i)
		result += m_combination[i].first * m_combination[i].second->shade(ray, hit, scene);
	return result;
}
