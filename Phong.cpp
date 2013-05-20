#include "Phong.h"
#include "Ray.h"
#include "Scene.h"

Phong::Phong(const Vector3 & color, const Vector3 &ks, const Vector3 &ka): m_kd(new Material(color)), m_ks(new Material(ks)), m_ka(ka), m_dp(1), m_sp(0), m_tp(0), m_phong(100), m_refr(1), m_phongp(0), m_indirect(true)
{
}

Phong::Phong(shared_ptr<Material> kd, shared_ptr<Material> ks, const Vector3 &ka) : m_kd(kd), m_ks(ks), m_ka(ka), m_dp(1), m_sp(0), m_tp(0), m_phong(100), m_refr(1), m_indirect(true)
{
}

Phong::Phong(shared_ptr<Material> kd, const Vector3 &ka) : m_kd(kd), m_ks(new Material(Vector3(1))), m_ka(ka), m_dp(1), m_sp(0), m_tp(0), m_phong(100), m_refr(1), m_indirect(true)
{
}

Phong::~Phong()
{
}

Vector3
Phong::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
        
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    Vector3 diffcolor = m_kd->shade(ray, hit, scene);
    Vector3 speccolor = m_ks->shade(ray, hit, scene);
	Vector3 randvect(randone(g_rng), randone(g_rng), randone(g_rng));
	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;

        float shadow_mul = 0.f;

        Vector3 tolight = pLight->position() - hit.P;
        tolight.normalize();
        Vector3 xaxis = randvect.cross(tolight);
        xaxis.normalize();
        Vector3 yaxis = xaxis.cross(tolight);
        yaxis.normalize();
        float xdisk, ydisk;
        do
        {
            xdisk = 1 - 2*randone(g_rng);
            ydisk = 1 - 2*randone(g_rng);
        } while (xdisk*xdisk + ydisk*ydisk > 1);
        Vector3 posinlight = pLight->position() + xdisk*pLight->sphere()*xaxis + ydisk*pLight->sphere()*yaxis;
        Ray lightray;
        lightray.d = (posinlight - hit.P);
        float raylength = lightray.d.length();
        float remaininglength = raylength;
        lightray.d /= raylength;
        lightray.o = hit.P;
        while (true)
        {
            HitInfo lighthit;

            if (!scene.trace(lighthit, lightray, EPSILON, remaininglength))
            {
                shadow_mul = 1;
                break;
            }
            if (lighthit.material->castShadow())
                break;
            remaininglength -= lighthit.t;
            lightray.o = lighthit.P;
        }

        if (shadow_mul < EPSILON)
            continue;

        // Diffuse
        Vector3 result = pLight->color()*shadow_mul;

        // the inverse-squared falloff
        float falloff = raylength*raylength;

        // normalize the light direction

        float nDotL = dot(hit.N, lightray.d);

        if (m_dp > EPSILON)
        {
            L += std::max(0.0f, nDotL/falloff * pLight->wattage() / PI) * result * diffcolor * m_dp;
        }
        if (m_phongp > EPSILON && nDotL > 0)
        {
            Vector3 refl = -lightray.d + 2*nDotL*hit.N;
            L += std::max(0.f, powf((-ray.d).dot(refl), m_phong)) * result * speccolor * m_phongp;
            //Vector3 half = -ray.d + hit.N;
            //half.normalize();
            //L += std::max(0.f, powf(half.dot(hit.N), 1000)) * result * speccolor * m_sp;
        }
    }

	// Indirect diffuse sampling (method 2)
	if (m_indirect && m_dp > EPSILON && ray.iter < MAX_RAY_ITER)
	{
		float theta = asinf((randone(g_rng)));
		float phi = 2 * PI * randone(g_rng);

		Vector3 const &yaxis = hit.N;
		Vector3 xaxis = yaxis.cross(randvect);
		xaxis.normalize();
		Vector3 zaxis = yaxis.cross(xaxis);
		zaxis.normalize();
		Ray diffuse_ray;
		diffuse_ray.iter = ray.iter + 1;
		diffuse_ray.o = hit.P;
		diffuse_ray.d = 0;
		diffuse_ray.d += sinf(theta) * cosf(phi) * xaxis;
		diffuse_ray.d += sinf(theta) * sinf(phi) * zaxis;
		diffuse_ray.d += cosf(theta) * yaxis;
        diffuse_ray.d.normalize();

		HitInfo diff_hit;
		Vector3 diff_res;
		if (scene.trace(diff_hit, diffuse_ray, EPSILON))
			diff_res = diff_hit.material->shade(diffuse_ray, diff_hit, scene);
		else
			diff_res = scene.bgColor();
		L += (m_dp / PI) * (diffcolor * diff_res);
	}

    if (m_sp > EPSILON && ray.iter < MAX_RAY_ITER)
    {
        Ray newray;
        newray.o = hit.P;
        newray.d = ray.d - 2*ray.d.dot(hit.N)*hit.N;
        newray.iter = ray.iter + 1;
        newray.refractionIndex = ray.refractionIndex;
        newray.refractionStack = ray.refractionStack;
        HitInfo minHit;
        if (scene.trace(minHit, newray, EPSILON))
        {
            L += minHit.material->shade(newray, minHit, scene) * m_sp;
        }
        else
        {
            L += scene.bgColor() * m_sp;
        }
    }

    if (m_tp > EPSILON && ray.iter < MAX_RAY_ITER)
    {
        Ray newray;
        newray.refractionStack = ray.refractionStack;
        newray.refractionIndex = ray.refractionIndex;
        float ratio = 1.0;
        if ((*ray.refractionStack)[ray.refractionIndex] == m_refr && m_refr != 1.0)
        {
            newray.refractionIndex = ray.refractionIndex - 1;
            ratio = m_refr / (*newray.refractionStack)[newray.refractionIndex];
        }
        else if ((*ray.refractionStack)[ray.refractionIndex] != m_refr)
        {
            newray.refractionIndex = ray.refractionIndex + 1;
            newray.refractionStack->push_back(m_refr);
            ratio = (*ray.refractionStack)[ray.refractionIndex] / m_refr;
        }
        newray.o = hit.P;
        Vector3 w = -ray.d;
        float dDotN = w.dot(hit.N);
        if (dDotN < 0)
            dDotN = -dDotN;
        newray.d = -ratio * (w - dDotN*hit.N) - sqrtf(1 - ratio*ratio*(1 - dDotN*dDotN)) * hit.N;
        newray.d.normalize();
        newray.iter = ray.iter + 1;
        HitInfo minHit;
        if (scene.trace(minHit, newray, EPSILON))
        {
            L += minHit.material->shade(newray, minHit, scene) * m_tp;
        }
        else
        {
            L += scene.bgColor() * m_tp;
        }
    }

    // We are assuming the diffuse color and ambient color are the same here.
    L += diffcolor * m_ka;

    return L;
}
