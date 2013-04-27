#ifndef CSE168_CELLULARNOISE_H_INCLUDED
#define CSE168_CELLULARNOISE_H_INCLUDED

#include <math.h>
#include "Vector3.h"
#include "Perlin.h"

class CellularNoise
{
public:
		static float noise(const Vector3& pos, float seed = 1.0f)
		{
			Vector3 thiscell = Vector3 (floor(pos.x) + 0.5, floor(pos.y)  +0.5, floor(pos.z) + 0.5);
			float f1 = 1000;
			float i, j, k;
			Vector3 pos1;

			for (i = -1;  i <= 1;  i += 1) 
			{
				for (j = -1;  j <= 1;  j += 1) 
				{
					for (k = -1;  k <= 1;  k += 1) 
					{
						Vector3 testcell = thiscell + Vector3(i, j, k);
						Vector3 pos2 = testcell + seed * (PerlinNoise::noise(testcell.x, testcell.y, testcell.z) - 0.5);
						Vector3 offset = pos2 - pos;
						float dist = offset . offset;
						if (dist < f1) 
						{
							f1 = dist;
							pos1 = pos;
						}
					}
				}
			}
			f1 = sqrt(f1);
			return f1;
		}

		static float euclidianDistance(Vector3 v1, Vector3 v2)
		{
			return (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z);
		}

		static float manhattanDistance(Vector3 v1, Vector3 v2)
		{
			return abs(v1.x - v2.x) + abs(v1.y - v2.y) + abs(v1.z - v2.z);
		}

};

#endif // CSE168_CELLULARNOISE_H_INCLUDED