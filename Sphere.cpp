#include "Sphere.h"
#include "Ray.h"
#include "Console.h"

Sphere::Sphere()
{
}

Sphere::~Sphere()
{
}

void
Sphere::renderGL()
{
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(m_center.x(), m_center.y(), m_center.z());
    glutWireSphere(m_radius, 20, 20);
    glPopMatrix();
}

bool Sphere::doIntersect(std::vector<void *> const &objects, HitInfo& result, const Ray& ray,
                         float tMin, float tMax)
{
#ifndef SSE
    const float a2 = 2.0f * ray.d.length2(); 
    const float a4 = 2.0f * a2;
#else
    __m128 a2;
    const int D = 3;
    DOTPRODUCT(a2, ray.d.mm, ray.d.mm);
    a2 = _mm_add_ps(a2, a2);
    __m128 a4;
    a4 = _mm_add_ps(a2, a2);
    a2 = _mm_rcp_ss(a2);
    __m128 signmask = _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, 0x80000000));
#endif
    const Vector3 ray2 = ray.d + ray.d;
    bool hit = false;

    for (size_t i = 0; i < objects.size(); ++i)
    {
        Sphere *s = (Sphere *) objects[i];

        const Vector3 toO = ray.o - s->m_center; 

#ifndef SSE
        const float b = dot(ray2, toO);
        const float c = toO.length2() - s->m_radius2;

        const float discrim = b*b-a4*c; 

        if (LIKELY(discrim < 0)) 
	    continue;
#else
        __m128 b;
        DOTPRODUCT(b, ray2.mm, toO.mm);
        __m128 radius2 = _mm_set_ss(s->m_radius2);
        __m128 c;
        DOTPRODUCT(c, toO.mm, toO.mm);
        c = _mm_sub_ss(c, radius2);

        __m128 discrim_sse = _mm_sub_ss(_mm_mul_ss(b, b), _mm_mul_ss(a4, c));

        if (LIKELY(_mm_testc_si128(_mm_castps_si128(discrim_sse), _mm_castps_si128(signmask))))
	    continue;
#endif

#ifndef SSE
        const float sqrt_discrim = sqrt(discrim); 

	// solve the quadratic equation
	const float t[2] = {(-b-sqrt_discrim)/(a2), (-b+sqrt_discrim)/(a2)}; 
#else
        discrim_sse = _mm_sqrt_ss(discrim_sse);
        b = _mm_xor_ps(b, signmask);
        __m128 t0 = _mm_mul_ss(_mm_sub_ss(b, discrim_sse), a2);
        __m128 t1 = _mm_mul_ss(_mm_add_ss(b, discrim_sse), a2);

        float t[2];
        _mm_store_ss(t, t0);
        _mm_store_ss(t + 1, t1);
#endif

        // since we know that discrim >= 0, t[0] < t{1]
        // return the t closest to us that is within range
        if ((t[0] > tMin) && (t[0] < tMax))
        {
            result.t = t[0];
        }
        else if((t[1] > tMin) && (t[1] < tMax))
        {
            result.t = t[1];
        }
        else
        {
            // neither of the solutions are in the required range
            continue; 
        }
        hit = true;
        tMax = result.t;

        result.P = ray.o + result.t*ray.d; 
        result.N = (result.P-s->m_center); 
        result.material = s->m_material; 
    }

    if (hit)
    {
        result.N.normalize();
    }

    return hit;
}

ObjType Sphere::type() const { return SPHERE; }

void *Sphere::selfPointer() { return this; }
