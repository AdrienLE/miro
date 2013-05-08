#include "BoundingBox.h"


BoundingBox::BoundingBox(const Vector3& a, const Vector3& b) : _a(a), _b(b)
{
}


BoundingBox::~BoundingBox(void)
{
}

bool	BoundingBox::doIntersect(HitInfo &, const Ray &, float, float)
{
	return false;
}
