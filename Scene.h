#ifndef CSE168_SCENE_H_INCLUDED
#define CSE168_SCENE_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "PointLight.h"
#include "BVH.h"

class Camera;
class Image;

class Scene
{
public:
    void addObject(Object* pObj)        {m_objects.push_back(pObj);}
    const Objects* objects() const      {return &m_objects;}

    void addLight(PointLight* pObj)     {m_lights.push_back(pObj);}
    const Lights* lights() const        {return &m_lights;}

    void preCalc();
    void openGL(Camera *cam);

    Scene() {setAntiAliasing(1, 1);}

    void raytraceImage(Camera *cam, Image *img);
    bool trace(HitInfo& minHit, const Ray& ray,
               float tMin = 0.0f, float tMax = MIRO_TMAX) const;

    void setAntiAliasing(int x, int y);

protected:
    std::vector<Vector3 *> traceLine(Camera const *cam, Image const *img, int j) const;

    Objects m_objects;
    BVH m_bvh;
    Lights m_lights;
    struct Vector2
    {
        float x;
        float y;
        Vector2(float _x, float _y) : x(_x), y(_y) {}
        Vector2() {}
    };
    std::vector<Vector2> m_antialiasing;
};

extern Scene * g_scene;

#endif // CSE168_SCENE_H_INCLUDED
