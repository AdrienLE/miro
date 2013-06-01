#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"
#include "Texture.h"

class Phong : public Material
{
public:
    Phong(const Vector3 & diffuse, const Vector3 &specular, const Vector3 &ambient);
	Phong(shared_ptr<Material> kd, shared_ptr<Material> ks, const Vector3 &ambient);
	Phong(shared_ptr<Material> kd, const Vector3 &ambient = Vector3(0));
    virtual ~Phong();

    void setPhong(float f) {m_phong = f;}
    void setRefraction(float indice, float proportion) {m_refr = indice; m_tp = proportion;}
    void setIndirectLighting(bool value) {m_indirect = value;}

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
	void setTransparency(float tp) { m_tp = tp; }
	void setReflectionIndex(float refr) { m_refr = refr; }
    
	void setKa(const Vector3& ka) { m_ka = ka; }
	void setKs(const Vector3& ks) { m_ks = ks; }
	const Vector3& getKs() { return m_ks; }
	void setKd(const Vector3& kd) { m_kd = kd; }
	void setBm(float bm) { m_bm = bm; }

	void setKaTexture(shared_ptr<Material> ka_texture) { m_texture_ka = ka_texture; }
	void setKsTexture(shared_ptr<Material> ks_texture) { m_texture_ks = ks_texture; }
	void setKdTexture(shared_ptr<Material> kd_texture) { m_texture_kd = kd_texture; }
	void setBumpTexture(shared_ptr<Texture> bump_texture) { m_texture_bump = bump_texture; }

protected:
	shared_ptr<Material> m_texture_ka;
    shared_ptr<Material> m_texture_ks;
	shared_ptr<Material> m_texture_kd;
	shared_ptr<Texture> m_texture_bump;

    Vector3 m_ka;
	Vector3 m_ks; // old: m_sp
	Vector3 m_kd; // old: m_dp
	float m_bm;

    float m_tp;
    float m_phong;
    float m_refr;
    bool m_indirect;
};

#endif // CSE168_LAMBERT_H_INCLUDED
