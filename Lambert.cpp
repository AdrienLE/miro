#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"

Lambert::Lambert(const Vector3 & color, const Vector3 &ka): m_kd(new Material(color)), m_ka(ka)
{
}

Lambert::Lambert(shared_ptr<Material> kd, const Vector3 &ka) : m_kd(kd), m_ka(ka)
{
}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
    
    const Vector3 viewDir = -ray.d; // d is a unit vector
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    Vector3 color = m_kd->shade(ray, hit, scene);
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;

        Vector3 l = pLight->position() - hit.P;

        Ray lightray;
        lightray.d = l.normalized();
        lightray.o = hit.P;
        HitInfo lighthit;

        if (scene.trace(lighthit, lightray, EPSILON, l.length()))
            continue;
        
        // the inverse-squared falloff
        float falloff = l.length2();
        
        // normalize the light direction
        l /= sqrt(falloff);

        // get the diffuse component
        float nDotL = dot(hit.N, l);
        Vector3 result = pLight->color();
		result *= color;
        
        L += std::max(0.0f, nDotL/falloff * pLight->wattage() / PI) * result;
    }

    L += color * m_ka;

    return L;
}
