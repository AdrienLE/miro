#pragma once

#include <vector>
#include <stdint.h>
#include <queue>
#include <boost/function.hpp>
#include "Vector3.h"
#include "Miro.h"

struct Photon
{
	Vector3 position;
	Vector3 power;
	Vector3 dir;
};

class PhotonMap
{
public:
	PhotonMap() : m_root(0) {}

	void addPhoton(Photon const &ph) {m_photons.push_back(new Photon(ph));}

	void build();

	void kNearest(Vector3 const &v, int k, std::vector<Photon *> &result) const;

	~PhotonMap() {delete m_root;}

private:
	struct PhotonNode
	{
		Photon *photon;
		PhotonNode *left;

		PhotonNode() : photon(0), left(0) {}

		~PhotonNode()
		{
			delete photon;
			if (left)
			{
				delete[] (PhotonNode*)((uintptr_t) left & ~3);
			}
		}
	};

	typedef std::priority_queue<Photon *, std::vector<Photon *>, boost::function<bool (Photon *, Photon *)> > Heap;
	void tryInsertPhoton(Vector3 const &p, int k, Photon *photon, Heap &result) const;
	void recKNearest(PhotonNode *node, Vector3 const &v, int k, Heap &result) const;
	void recursiveBuild(PhotonNode *node, int axis, std::vector<Photon*>::iterator start, std::vector<Photon*>::iterator end);

	std::vector<Photon *> m_photons;
	PhotonNode *m_root;
};