#include "CellularStoneTexture.h"
#include "Ray.h"
#include "Scene.h"
#include "Perlin.h"
#include "Worley.h"

CellularStoneTexture::CellularStoneTexture(const Vector3 & color) : m_color(color)
{
	preCalc();
}

CellularStoneTexture::~CellularStoneTexture()
{
}

void
CellularStoneTexture::preCalc()
{
	
}

Vector3 
CellularStoneTexture::getTextureColorAt(const Vector3& pos) const
{
	Vector3 color(0);
	Vector3 npos(pos);

	Vector3 slateGray(112/255., 128/255., 144/255.);
	Vector3 lightBlue(104/255., 131/255., 139/255.);
	Vector3 darkWood(.52, 94/255., 66/255.);
	Vector3 lightSteelBlue(.33, .33, .33);
	Vector3 colors[] = { slateGray, lightBlue, darkWood, lightSteelBlue };

	const int max_order = 3;
	float scale = 1;
	float at[] = {npos.x, npos.y, npos.z};
	float F[max_order];
	float delta[max_order][3];
	unsigned long ID[max_order];
	float sep_delta = 0.05f;

	WorleyNoise::noise3D(at, max_order, F, delta, ID); 
	if(F[2] - F[1] < -sep_delta || F[2] - F[1] > sep_delta) 
	{
		int temp = ID[1] % 4;
		int temp2 = ID[0] % 4;
		

		//float noiseX = PerlinNoise::noise(npos[0] * scale, 
		//								  npos[1] * scale, 
		//								  npos[2] * scale);
		//float noiseY = PerlinNoise::noise(npos[1] * scale, 
		//							      npos[2] * scale, 
		//								  npos[0] * scale);
		//float noiseZ = PerlinNoise::noise(npos[2] * scale, 
		//								  npos[0] * scale, 
		//								  npos[1] * scale);
		//interpolate the bumps using a predefined scale
		//float bumpScale = .4;
		//pNorm.x = (1 - bumpScale) * normal.x + bumpScale * noiseX;
		//pNorm.y = (1 - bumpScale) * normal.y + bumpScale * noiseY;
		//pNorm.z = (1 - bumpScale) * normal.z + bumpScale * noiseZ;

		color = colors[temp] + colors[temp2];
	}

	return color;
}

Vector3
CellularStoneTexture::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	return getTextureColorAt(hit.P);
}
