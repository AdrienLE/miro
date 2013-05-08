#pragma once
#include "object.h"

class BoundingBox
{
public:
	BoundingBox(const Vector3& a, const Vector3& b);
	virtual ~BoundingBox();

	bool doIntersect(HitInfo &, const Ray &, float, float);
    ObjType type() const { return BOUNDING_BOX; }

protected:
    Vector3 _a;
	Vector3 _b;
};

