#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include <time.h>
#include "boost/threadpool.hpp"

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
    for (int i = 0; i < img->width(); ++i)
    {
        ray = cam->eyeRay(i, j, img->width(), img->height());
        if (trace(hitInfo, ray))
        {
            results[i] = new Vector3(hitInfo.material->shade(ray, hitInfo, *this));
        }
    }
    return results;
}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
    time_t t = time(0);
    boost::threadpool::pool threadpool(10);
    std::vector<boost::packaged_task<std::vector<Vector3 *> > * > tasks;
    std::vector<boost::unique_future<std::vector<Vector3 *> > * > lines;

    // loop over all pixels in the image
    for (int j = 0; j < img->height(); ++j)
    {
        tasks.push_back(new boost::packaged_task<std::vector<Vector3 *> >(boost::bind(&Scene::traceLine, this, cam, img, j)));
        lines.push_back(new boost::unique_future<std::vector<Vector3 *> >(tasks.back()->get_future()));
        boost::threadpool::schedule(threadpool, boost::bind(&boost::packaged_task<std::vector<Vector3 *> >::operator(), tasks.back()));
    }
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
    
    printf("Rendering Progress: 100.000%\n");
    debug("done Raytracing!\n");
    printf("%ds\n", time(0) - t);
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}
