#include "TriMesh.h"

TriMesh::TriMesh(ISceneNode *irrobject, double mass)
	   :PhysicalObject(irrobject, mass)
{
	trimesh = new btTriangleMesh();

	IMeshBuffer* mbuffer = ((IAnimatedMeshSceneNode*)irrobject)->getMesh()->getMeshBuffer(0);

	int numIndices = mbuffer->getIndexCount();

	int indexBase[numIndices];

	u16* irrIndexBase = mbuffer->getIndices();

	for(int i = 0; i < numIndices; i++)
		indexBase[i] = (int)irrIndexBase[i];

	int numVertices = mbuffer->getVertexCount();

	btVector3 vertexBase[numVertices];

	S3DVertex* vertices = (S3DVertex*)(mbuffer->getVertices());
	vector3df scale = irrobject->getScale();

	for(int i = 0; i < numVertices; i++)
	{
		vertexBase[i].setX((float)(vertices[i].Pos.X * scale.X));
		vertexBase[i].setY((float)(vertices[i].Pos.Y * scale.Y));
		vertexBase[i].setZ((float)(vertices[i].Pos.Z * scale.Z));
	}

	for(int i = 0; i < numIndices; i += 3)
	{
		trimesh->addTriangle( vertexBase[indexBase[i]], vertexBase[indexBase[i+1]], vertexBase[indexBase[i+2]]);
	}

	btGImpactMeshShape* trimesh_sh = new btGImpactMeshShape(trimesh);
	trimesh_sh->updateBound();

	initialize(trimesh_sh);

}

TriMesh::~TriMesh()
{
	delete trimesh;
}
