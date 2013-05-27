#include "PhotonMap.h"
#include <boost/timer/timer.hpp>

#define SANITIZE(ptr) ((PhotonNode*)(((uintptr_t)ptr) & ~3))

void PhotonMap::recursiveBuild(PhotonNode *node, int axis, std::vector<Photon*>::iterator start, std::vector<Photon*>::iterator end)
{
	int n = end - start;
	if (n == 0)
		return;
	if (n == 1)
	{
		node->photon = *start;
		return;
	}
	std::vector<Photon*>::iterator a = start + rand() % n;
	std::vector<Photon*>::iterator b = start + rand() % n;
	std::vector<Photon*>::iterator c = start + rand() % n;
	float ap = (*a)->position[axis];
	float bp = (*b)->position[axis];
	float cp = (*c)->position[axis];
	std::vector<Photon*>::iterator median;
	if (ap > std::min(bp, cp) && ap < std::max(bp, cp))
		median = a;
	else if (bp > std::min(ap, cp) && bp < std::max(ap, cp))
		median = b;
	else
		median = c;
	std::swap(*median, *start);
	std::vector<Photon*>::iterator middle = start + (n - 1);
	for (std::vector<Photon*>::iterator it = start + 1; it != middle;)
	{
		if ((*it)->position[axis] <= (*start)->position[axis])
		{
			++it;
		}
		else
		{
			std::swap(*it, *middle);
			--middle;
		}
	}
	PhotonNode *realptr = new PhotonNode[2];
	uintptr_t ptr = (uintptr_t) realptr;
	ptr |= axis;
	node->left = (PhotonNode *) ptr;
	node->photon = *start;
	recursiveBuild(realptr, (axis + 1) % 3, start + 1, middle);
	recursiveBuild(realptr + 1, (axis + 1) % 3, middle, end);
}

void PhotonMap::recKNearest(PhotonNode *node, Vector3 const &p, int k, std::vector<Photon *> &result) const
{
	if (!node->photon)
		return;
	if (!node->left)
	{
		tryInsertPhoton(p, k, node->photon, result);
		return;
	}
	int axis = ((uintptr_t)node->left) & 3;
	PhotonNode *left = SANITIZE(node->left);
	PhotonNode *right = left + 1;
	if (p[axis] > node->photon->position[axis])
		std::swap(left, right);
	recKNearest(left, p, k, result);
	tryInsertPhoton(p, k, node->photon, result);
	if (result.size() < k || (result.back()->position - p).length2() >= (p[axis] - node->photon->position[axis])*(p[axis] - node->photon->position[axis]))
		recKNearest(right, p, k, result);
}

void PhotonMap::kNearest(Vector3 const &p, int k, std::vector<Photon *> &result) const
{
	result.clear();
	recKNearest(m_root, p, k, result);
}

void PhotonMap::tryInsertPhoton(Vector3 const &p, int k, Photon *photon, std::vector<Photon *> &result) const
{
	if (result.size() < k)
		result.push_back(photon);
	else if ((result.back()->position - p).length2() > (photon->position - p).length2())
		result.back() = photon;
	else
		return;
	for (int i = result.size() - 1; i > 0; --i)
	{
		if ((result[i]->position - p).length2() < (result[i - 1]->position - p).length2())
			std::swap(result[i], result[i - 1]);
		else
			break;
	}
}

void PhotonMap::build()
{
	m_root = new PhotonNode();
	recursiveBuild(m_root, 0, m_photons.begin(), m_photons.end());
	m_photons.clear();
}

// boost::mt19937 g_rng;


// int main()
// {
// 	PhotonMap map;
// 	std::vector<Photon> photons;
// 	for (int i = 0; i < 30000000; ++i)
// 	{
// 		Photon p;
// 		p.position = Vector3(rand() * randone(g_rng), rand() * randone(g_rng), rand() * randone(g_rng));
// 		photons.push_back(p);
// 		map.addPhoton(p);
// 	}
// 	{
// 	    boost::timer::auto_cpu_timer t;
// 	map.build();
// }
// 	Vector3 pos(rand() * randone(g_rng), rand()*randone(g_rng), rand()*randone(g_rng));
// 	std::vector<Photon *> res;
// 	{
// 	    boost::timer::auto_cpu_timer t;
// 		map.kNearest(pos, 10, res);
// 	}
// 	std::sort(photons.begin(), photons.end(), [&](Photon const &a, Photon const &b) {
// 		return (a.position - pos).length2() < (b.position - pos).length2();
// 	});
// 	for (int i = 0; i < res.size(); ++i)
// 	{
// 		if (res[i]->position != photons[i].position)
// 			printf("error %f / %f\n", (res[i]->position - pos).length(), (photons[i].position - pos).length());
// 	}
// 	return 0;
// }
