#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}

void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}

bool Triangle::doIntersect(IntersectObjects const &objects, HitInfo& result,
                           const Ray& ray, float tMin, float tMax)
{
    bool hit = false;
    Vector3 md = -ray.d;
    for (int i = 0; i < objects.plain.size(); ++i)
    {
        Triangle *tri = (Triangle *)objects.plain[i];
        Vector3 *vs = tri->m_mesh->vertices();
        Vector3 *ns = tri->m_mesh->normals();
        TriangleMesh::TupleI3 *vi = tri->m_mesh->vIndices();
        TriangleMesh::TupleI3 *ni = tri->m_mesh->nIndices();

        for (int j = 0; j < tri->m_mesh->numTris(); ++j)
        {
            Vector3 ab = vs[vi[j].y] - vs[vi[j].x];
            Vector3 ac = vs[vi[j].z] - vs[vi[j].x];
            Vector3 normal = ab.cross(ac);
            Vector3 o_a = ray.o - vs[vi[j].x];
            float detA = 1 / md.dot(normal);
            float t = o_a.dot(normal) * detA;
            float alpha = md.dot(o_a.cross(ac)) * detA;
            float beta = md.dot(ab.cross(o_a)) * detA;
            if (alpha >= 0 && beta >= 0 && alpha + beta <= 1 && t > tMin && t < tMax)
            {
                result.t = t;
                result.P = ray.o + result.t*ray.d;
                Vector3 na = ns[ni[j].x];
                Vector3 nb = ns[ni[j].y];
                Vector3 nc = ns[ni[j].z];
                result.N = (1 - alpha - beta) * na + alpha * nb + beta * nc;
                // result.N = normal;
                result.N.normalize();
                result.material = tri->m_material;
                tMax = result.t;
                hit = true;
            }
        }
    }
    return hit;
}

SSEObject * Triangle::preProcess( std::vector<void *> &objects )
{
    SSETriangles *triangles = new SSETriangles;
    return NULL;
}


// bool Triangle::doIntersect(std::vector<void *> const &objects, HitInfo& result,
//                            const Ray& ray, float tMin, float tMax)
// {
//     const int neg1 = -1;
//     Vector3 d = ray.d;
//     d.negate();
//     Vector3 *vs;
//     Vector3 *ns;
//     TriangleMesh::TupleI3 *vi;
//     TriangleMesh::TupleI3 *ni;
//     const __m128 one = _mm_set_ps1(1);
//     bool hit = false;
//     // This is ridiculous, need a better way to do that
//     const int D = 3;
//     union
//     {
//         __m128 m;
//         struct
//         {
//             float a;
//             float b;
//             float t;
//             int i;
//         };
//     } min;
//     min.m = _mm_setzero_ps();

//     for (int j = 0; j < objects.size(); ++j)
//     {
//         Triangle *triangle = (Triangle *) objects[j];
//         vs = triangle->m_mesh->vertices();
//         ns = triangle->m_mesh->normals();
//         vi = triangle->m_mesh->vIndices();
//         ni = triangle->m_mesh->nIndices();
//         // TODO: handle EPSILON, somehow use tMin...
//         for (int i = 0; i < triangle->m_mesh->numTris(); ++i)
//         {
//             Vector3 ab = vs[vi[i].y] - vs[vi[i].x];
//             Vector3 ac = vs[vi[i].z] - vs[vi[i].x];
//             Vector3 normal = ab.cross(ac);
//             Vector3 o_a = ray.o - vs[vi[i].x];
//             int ji = (j << 10) | i + 1;

//             // This is the classical triangle intersection thing, nothing crazy here
//             // The result is stored the same way as the union that contains min
//             __m128 deta;
//             LOCATED_DOTPRODUCT(deta, d.mm, normal.mm, 0xf);
//             deta = _mm_rcp_ps(deta);
//             __m128 tmp, result = _mm_set_ps(*(float *)&ji, 0, 0, 0);
//             LOCATED_DOTPRODUCT(tmp, o_a.mm, normal.mm, 4);
//             result = _mm_or_ps(tmp, result);
//             LOCATED_DOTPRODUCT(result, d.mm, o_a.cross(ac).mm, 1);
//             result = _mm_or_ps(tmp, result);
//             LOCATED_DOTPRODUCT(result, d.mm, ab.cross(o_a).mm, 2);
//             result = _mm_or_ps(tmp, result);
//             result = _mm_mul_ps(result, deta);

//             // Now we are starting to go crazy: I don't want any ifs in this code,
//             // so this uses a bunch of crazy stuff to avoid it.
//             __m128 mask = _mm_cmpnge_ps(result, min.m);
//             // If there is any 0xffffffff this will propagate it everywhere
//             mask = _mm_hadd_ps(mask, mask);
//             mask = _mm_hadd_ps(mask, mask);
//             // We tested NOT superior or equal, but we really want superior or equal
//             mask = _mm_andnot_ps(mask, mask);

//             // Testing whether a + b <= 1
//             __m128 testab = _mm_hadd_ps(result, result);
//             testab = _mm_cmple_ss(testab, one);
//             testab = _mm_shuffle_ps(testab, testab, _MM_SHUFFLE(0, 0, 0, 0));

//             // Creating the real mask
//             mask = _mm_and_ps(testab, mask);

//             min.m = _mm_and_ps(min.m, _mm_andnot_ps(mask, mask));
//             min.m = _mm_or_ps(min.m, _mm_and_ps(result, mask));
//         }
//     }
//     if (min.i != 0 && min.t < tMax)
//     {
//         result.t = min.t;
//         result.P = ray.o + result.t*ray.d;
//         int j = min.i >> 10;
//         int i = min.i ^ (j << 10);
//         Triangle *triangle = (Triangle *) objects[j];
//         vs = triangle->m_mesh->vertices();
//         vi = triangle->m_mesh->vIndices();
//         Vector3 ab = vs[vi[min.i].y] - vs[vi[min.i].x];
//         Vector3 ac = vs[vi[min.i].z] - vs[vi[min.i].x];
//         // TODO: normal interpolation and shit.
//         result.N = ab.cross(ac);
//         result.material = triangle->m_material;
//         tMax = min.t;
//         hit = true;
//     }
//     std::cout << "here\n";
//     return hit;
// }
