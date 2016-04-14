#ifndef TRIMESH_H
#define TRIMESH_H

#include "PhysicalObject.h"

#include <BulletCollision/Gimpact/btGImpactShape.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

class TriMesh : public PhysicalObject
{
	private:
		btTriangleMesh* trimesh;

	public:
		TriMesh(ISceneNode *irrobject, double mass);
		~TriMesh();
};

#endif // TRIMESH_H
