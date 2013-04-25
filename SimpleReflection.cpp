#include "SimpleReflection.h"
#include "Ray.h"
#include "Scene.h"

SimpleReflection::SimpleReflection() {}
SimpleReflection::~SimpleReflection() {}

Vector3 SimpleReflection::shade(const Ray &ray, const HitInfo &hit,
								const Scene &scene) const
{
	if (ray.iter >= MAX_RAY_ITER)
		return Vector3();
	Ray reflRay;
	reflRay.o = hit.P;
	reflRay.d = hit.N;
	reflRay.iter = ray.iter + 1;
	HitInfo reflHit;
	if (scene.trace(reflHit, reflRay, EPSILON))
		return reflHit.material->shade(reflRay, reflHit, scene);
	return Vector3();
}
