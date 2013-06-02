#include "Phong.h"
#include "Ray.h"
#include "Scene.h"

Phong::Phong(const Vector3 & kd, const Vector3 &ks, const Vector3 &ka)
{
	m_ka = ka;
	m_ks = ks; // old: m_sp
	m_kd = kd; // old: m_dp
	m_bm = 1.0f;
	m_tp = 0;
	m_phong = 100;
	m_refr = 1;
	m_indirect = true;
	m_texture_ka = shared_ptr<Material>(new Material());
	m_texture_ks = shared_ptr<Material>(new Material());
	m_texture_kd = shared_ptr<Material>(new Material());
	m_texture_bump = NULL;
	m_is_glossy = false;
}

Phong::Phong(shared_ptr<Material> texture_kd, shared_ptr<Material> texture_ks, const Vector3 &ka)
{
	m_ka = ka;
	m_ks = Vector3(0.f);
	m_kd = Vector3(1.f);
	m_bm = 1.0f;
	m_tp = 0;
	m_phong = 100;
	m_refr = 1;
	m_indirect = true;
	m_texture_kd = texture_kd;
	m_texture_ks = texture_ks;
	m_texture_ka = shared_ptr<Material>(new Material());
	m_texture_bump = NULL;
	m_is_glossy = false;
}

Phong::Phong(shared_ptr<Material> texture_kd, const Vector3 &ka)
{
	m_ka = ka;
	m_ks = Vector3(0.f);
	m_kd = Vector3(1.f);
	m_bm = 1.0f;
	m_tp = 0;
	m_phong = 100;
	m_refr = 1;
	m_indirect = true;
	m_texture_kd = texture_kd;
	m_texture_ks = shared_ptr<Material>(new Material());
	m_texture_ka = shared_ptr<Material>(new Material());
	m_texture_bump = NULL;
	m_is_glossy = false;
}

Phong::~Phong()
{
}

Vector3
Phong::shade(const Ray& ray, const HitInfo& rhit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
 	Vector3 randvect(randone(g_rng), randone(g_rng), randone(g_rng));
    const Lights *lightlist = scene.lights();
    
	HitInfo bm_hit = rhit;
	bm_hit.N.normalize();
	// Bump mapping
	if (m_texture_bump != NULL)
	{
		Vector3 v_bump = m_bm * m_texture_bump->shade(ray, rhit, scene);
		Vector3 grads = m_bm * m_texture_bump->computeGradient(ray, rhit, scene);

		Vector3 const &py = bm_hit.N;
		Vector3 pu = py.cross(randvect);
		Vector3 pv = py.cross(pu);

		//bm_hit.N = bm_hit.N + (grads[1] * (bm_hit.N.cross(pv)) - grads[0] * (bm_hit.N.cross(pu))) / bm_hit.N.length();
		bm_hit.N = pu.cross(pv);
		bm_hit.N.normalize();
	}


    // loop over all of the lights
    Lights::const_iterator lightIter;
	Vector3 diffcolor = m_kd * m_texture_kd->shade(ray, bm_hit, scene);
    Vector3 speccolor = m_ks * m_texture_ks->shade(ray, bm_hit, scene);
	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;

        float shadow_mul = 0.f;

        Vector3 tolight = pLight->position() - bm_hit.P;
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
        lightray.d = (posinlight - bm_hit.P);
        float raylength = lightray.d.length();
        float remaininglength = raylength;
        lightray.d /= raylength;
        lightray.o = bm_hit.P;
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

        float nDotL = dot(bm_hit.N, lightray.d);

        if (m_kd.max() > EPSILON)
        {
            L += std::max(0.0f, nDotL/falloff * pLight->wattage() / PI) * result * diffcolor;
        }
        if (m_ks.max() > EPSILON && nDotL > 0)
        {
            Vector3 refl = -lightray.d + 2*nDotL*bm_hit.N;
            L += std::max(0.f, powf((-ray.d).dot(refl), m_phong)) * result * speccolor;
            //Vector3 half = -ray.d + hit.N;
            //half.normalize();
            //L += std::max(0.f, powf(half.dot(hit.N), 1000)) * result * speccolor * m_sp;
        }
    }

	// Indirect diffuse sampling (method 2)
	if (m_indirect && m_kd.max() > EPSILON && ray.iter < MAX_RAY_ITER)
	{
		float theta = asinf((randone(g_rng)));
		float phi = 2 * PI * randone(g_rng);
		Vector3 const &yaxis = bm_hit.N;
		Vector3 xaxis = yaxis.cross(randvect);
		xaxis.normalize();
		Vector3 zaxis = yaxis.cross(xaxis);
		zaxis.normalize();
		Ray diffuse_ray;
        diffuse_ray.refractionIndex = ray.refractionIndex;
        diffuse_ray.refractionStack = ray.refractionStack;
		diffuse_ray.iter = ray.iter + 1;
		diffuse_ray.o = bm_hit.P;
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
		L += (diffcolor * diff_res) / PI;
	}

    if (m_ks.max() > EPSILON && ray.iter < MAX_RAY_ITER)
    {
        Ray newray;
		newray.iter = ray.iter + 1;
		newray.refractionIndex = ray.refractionIndex;
		newray.refractionStack = ray.refractionStack;
		newray.o = bm_hit.P;

		if (m_is_glossy)
		{
			Vector3 R = ray.d - 2 * ray.d.dot(bm_hit.N) * bm_hit.N;

			Vector3 u = R.cross(randvect);
			u.normalize();
			Vector3 v = R.cross(u);
			v.normalize();

			float theta = acos(powf(1.0f - randone(g_rng), (1.0f / (m_phong + 1))));
			float phi = 2.0f * PI * randone(g_rng);
					
			newray.d = 0;
			newray.d += sinf(theta) * cosf(phi) * u;
			newray.d += sinf(theta) * sinf(phi) * v;
			newray.d += cosf(theta) * R;
		}
		else
			newray.d = ray.d - 2 * ray.d.dot(bm_hit.N) * bm_hit.N;

		newray.d.normalize();
        HitInfo minHit;
        if (scene.trace(minHit, newray, EPSILON))
        {
			L += minHit.material->shade(newray, minHit, scene) * speccolor;
        }
        else
        {
            L += scene.bgColor() * speccolor;
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
        newray.o = bm_hit.P;
        Vector3 w = -ray.d;
        float dDotN = w.dot(bm_hit.N);
        if (dDotN < 0)
            dDotN = -dDotN;
        newray.d = -ratio * (w - dDotN*bm_hit.N) - sqrtf(1 - ratio*ratio*(1 - dDotN*dDotN)) * bm_hit.N;
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
	L += m_ka * m_texture_ka->shade(ray, bm_hit, scene);

    return L;
}
