#include "Phong.h"
#include "Ray.h"
#include "Scene.h"

Phong::Phong(const Vector3 & color, const Vector3 &ks, const Vector3 &ka): m_kd(new Material(color)), m_ks(new Material(ks)), m_ka(ka), m_dp(1), m_sp(0), m_tp(0), m_phong(10), m_refr(1)
{
}

Phong::Phong(shared_ptr<Material> kd, shared_ptr<Material> ks, const Vector3 &ka) : m_kd(kd), m_ks(ks), m_ka(ka), m_dp(1), m_sp(0), m_tp(0), m_phong(10), m_refr(1)
{
}

Phong::Phong(shared_ptr<Material> kd, const Vector3 &ka) : m_kd(kd), m_ks(new Material(Vector3(1))), m_ka(ka), m_dp(1), m_sp(0), m_tp(0), m_phong(10), m_refr(1)
{
}

Phong::~Phong()
{
}

Vector3
Phong::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
    
    const Vector3 viewDir = -ray.d; // d is a unit vector
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    Vector3 diffcolor = m_kd->shade(ray, hit, scene);
    Vector3 speccolor = m_ks->shade(ray, hit, scene);
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;

        Vector3 l = pLight->position() - hit.P;

        float shadow_mul = 0.f;

        for (int i = 0; i < pLight->samples(); ++i)
        {
            Ray lightray;
            lightray.d = (((pLight->sphere()*(Vector3(randone(), randone(), randone())-Vector3(0.5)))+pLight->position()) - hit.P).normalized();
            lightray.o = hit.P;
            HitInfo lighthit;

            if (!scene.trace(lighthit, lightray, EPSILON, l.length()))
                shadow_mul += 1.f/pLight->samples();
        }

        if (shadow_mul < EPSILON)
            continue;

        // Diffuse
        Vector3 result = pLight->color()*shadow_mul;

        // the inverse-squared falloff
        float falloff = l.length2();

        // normalize the light direction
        l /= sqrt(falloff);

        float nDotL = dot(hit.N, l);

        if (m_dp > EPSILON)
        {
            L += std::max(0.0f, nDotL/falloff * pLight->wattage() / PI) * result * diffcolor * m_dp;
        }
        if (m_sp > EPSILON && nDotL > 0)
        {
            Vector3 normal = hit.N;
            Vector3 refl = -l + 2*nDotL*hit.N;
            L += std::max(0.f, powf((-ray.d).dot(refl), m_phong)) * result * speccolor * m_sp;
            //Vector3 half = -ray.d + hit.N;
            //half.normalize();
            //L += std::max(0.f, powf(half.dot(hit.N), 1000)) * result * speccolor * m_sp;
        }
    }

    if (m_sp > EPSILON && ray.iter < MAX_RAY_ITER)
    {
        Ray newray;
        newray.o = hit.P;
        newray.d = -ray.d + 2*ray.d.dot(hit.N)*hit.N;
        newray.iter = ray.iter + 1;
        newray.refractionIndex = ray.refractionIndex;
        newray.refractionStack = ray.refractionStack;
        HitInfo minHit;
        if (scene.trace(minHit, newray, EPSILON))
        {
            L += minHit.material->shade(newray, minHit, scene) * m_sp;
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
    }

    // We are assuming the diffuse color and ambient color are the same here.
    L += diffcolor * m_ka;

    return L;
}
