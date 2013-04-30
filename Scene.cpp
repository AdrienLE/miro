#include "boost/threadpool.hpp"
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include <boost/timer/timer.hpp>

Scene * g_scene = 0;

void
Scene::openGL(Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam->drawGL();

    // draw objects
    for (size_t i = 0; i < m_objects.size(); ++i)
        m_objects[i]->renderGL();

    glutSwapBuffers();
}

void
Scene::preCalc()
{
    Objects::iterator it;
    for (it = m_objects.begin(); it != m_objects.end(); it++)
    {
        Object* pObject = *it;
        pObject->preCalc();
    }
    Lights::iterator lit;
    for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
    {
        PointLight* pLight = *lit;
        pLight->preCalc();
    }

    m_bvh.build(&m_objects);
}

std::vector<Vector3 *> Scene::traceLine(Camera const *cam, Image const *img, int j) const
{
    std::vector<Vector3 *> results(img->width());
    Ray ray;
    HitInfo hitInfo;
    Vector3 shadeResult;
    std::vector<float> refr_stack;
    for (int i = 0; i < img->width(); ++i)
    {
        for (int k = 0; k < m_antialiasing.size(); ++k)
        {
            ray = cam->eyeRay(i + m_antialiasing[k].x, j + m_antialiasing[k].y, img->width(), img->height());
            refr_stack.clear();
            refr_stack.push_back(1.f);
            ray.refractionStack = &refr_stack;
            ray.refractionIndex = 0;
            if (!results[i])
                results[i] = new Vector3();
            if (trace(hitInfo, ray))
            {
                *results[i] += hitInfo.material->shade(ray, hitInfo, *this);
            }
            else
            {
                *results[i] += bgColor();
            }
        }
        if (results[i])
            *results[i] /= m_antialiasing.size();
    }
    return results;
}

void Scene::setAntiAliasing(int x, int y)
{
    m_antialiasing.clear();
    for (int i = 0; i < x; ++i)
        for (int j = 0; j < y; ++j)
            m_antialiasing.push_back(Vector2(((float)i)/x - 0.5f, ((float)j)/y - 0.5f));
    // Wikipedia says these constants are good...
    const float angle = atanf(0.5f);
    const float scale = sqrtf(5.f)/2.f;
    for (int i = 0; i < m_antialiasing.size(); ++i)
    {
        Vector2 n;
        n.x = (m_antialiasing[i].x * cosf(angle) - m_antialiasing[i].y * sinf(angle)) * scale;
        n.y = (m_antialiasing[i].x * sin(angle) + m_antialiasing[i].y * cosf(angle)) * scale;
        m_antialiasing[i] = n;
    }
}

inline int nCpus()
{
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );

    return sysinfo.dwNumberOfProcessors;
#else
    return sysconf( _SC_NPROCESSORS_ONLN );
#endif
}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
    boost::timer::auto_cpu_timer t;
    boost::threadpool::pool threadpool(nCpus() * 2);
    std::vector<boost::packaged_task<std::vector<Vector3 *> > * > tasks;
    std::vector<boost::unique_future<std::vector<Vector3 *> > * > lines;

    // loop over all pixels in the image
    {
        for (int j = 0; j < img->height(); ++j)
        {
            tasks.push_back(new boost::packaged_task<std::vector<Vector3 *> >(boost::bind(&Scene::traceLine, this, cam, img, j)));
            lines.push_back(new boost::unique_future<std::vector<Vector3 *> >(tasks.back()->get_future()));
            boost::threadpool::schedule(threadpool, boost::bind(&boost::packaged_task<std::vector<Vector3 *> >::operator(), tasks.back()));
        }
	printf("Rendering Progress: %.3f%%\r", 0.f);
        for (int j = 0; j < img->height(); ++j)
        {
            for (int i = 0; i < img->width(); ++i)
            {
                if (lines[j]->get()[i])
                {
                    img->setPixel(i, j, *lines[j]->get()[i]);
                    delete lines[j]->get()[i];
                }
            }
	    img->drawScanline(j);
	    if (j + 1 == img->height() || !lines[j + 1]->has_value())
	      glFinish();
            printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
            fflush(stdout);
        }
    }
    printf("Rendering Progress: 100.000%\n");
    debug("done Raytracing!\n");
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}
