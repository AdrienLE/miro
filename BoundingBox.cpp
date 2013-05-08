#include "BoundingBox.h"
#include "Ray.h"
#include <algorithm>

BoundingBox::BoundingBox(const Vector3& a, const Vector3& b) : _a(a), _b(b)
{
}


BoundingBox::~BoundingBox(void)
{
}

bool BoundingBox::doIntersect(HitInfo &result, const Ray &ray, float tMin, float tMax)
{
	Vector3 t1 = (_a - ray.o) / ray.d;
	Vector3 t2 = (_b - ray.o) / ray.d;
	
	float t_min = std::max(std::min(t1.x, t2.x), std::min(t1.y, t2.y), std::min(t1.z, t2.z));
	float t_max = std::min(std::max(t1.x, t2.x), std::max(t1.y, t2.y), std::max(t1.z, t2.z));

	if (t_max < 0 || t_min > t_max)
		return false;
	result.t = t_min < 0 ? t_max : t_min;
	result.P = ray.o + (ray.d * result.t);
	Vector3 p = (_a + _b) / 2.f;
	if (std::abs(p.x) >= std::abs(p.y) && std::abs(p.x) >= std::abs(p.z)) 
	{
		p.y = 0;
		p.z = 0;
	}
	else if (std::abs(p.y) >= std::abs(p.x) && std::abs(p.y) >= std::abs(p.z))
	{
		p.x = 0;
		p.z = 0;
	}
	else
	{
		p.x = 0;
		p.y = 0;
	}
	p.normalize();
	result.N = p;
	return true;
}
