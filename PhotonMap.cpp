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
	std::vector<Photon*>::iterator middle = std::partition(start + 1, end, [&](Photon *phot)
	{
		return phot->position[axis] < (*start)->position[axis];
	});
	PhotonNode *realptr = new PhotonNode[2];
	uintptr_t ptr = (uintptr_t) realptr;
	ptr |= axis;
	node->left = (PhotonNode *) ptr;
	node->photon = *start;
	recursiveBuild(realptr, (axis + 1) % 3, start + 1, middle);
	recursiveBuild(realptr + 1, (axis + 1) % 3, middle, end);
}

void PhotonMap::recKNearest(PhotonNode *node, Vector3 const &p, int k, Heap &result) const
{
	if (!node->photon)
		return;
	if (node->left)
	{
		int axis = ((uintptr_t)node->left) & 3;
		float dist = p[axis] - node->photon->position[axis];
		if (dist < 0)
		{
			recKNearest(SANITIZE(node->left), p, k, result);
			if (result.size() < k || dist*dist < (result.top()->position - p).length2())
				recKNearest(SANITIZE(node->left) + 1, p, k, result);
		}
		else
		{
			recKNearest(SANITIZE(node->left) + 1, p, k, result);
			if (result.size() < k || dist*dist < (result.top()->position - p).length2())
				recKNearest(SANITIZE(node->left), p, k, result);
		}
	}
	tryInsertPhoton(p, k, node->photon, result);
}

void PhotonMap::kNearest(Vector3 const &p, int k, std::vector<Photon *> &result) const
{
	result.clear();
	Heap priority_queue([&](Photon* a, Photon *b)
	{
		return (a->position - p).length2() < (a->position - p).length2();
	},
	result);
	recKNearest(m_root, p, k, priority_queue);
}

void PhotonMap::tryInsertPhoton(Vector3 const &p, int k, Photon *photon, Heap &result) const
{
	if (result.size() < k)
		result.push(photon);
	else if ((result.top()->position - p).length2() > (photon->position - p).length2())
	{
		result.pop();
		result.push(photon);
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
// 	for (int i = 0; i < 10000000; ++i)
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
// 		map.kNearest(pos, 10000000, res);
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
