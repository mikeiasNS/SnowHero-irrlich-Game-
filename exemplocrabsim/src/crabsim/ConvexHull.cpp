#include "ConvexHull.h"

ConvexHull::ConvexHull(ISceneNode *irrobject, double mass)
	   :PhysicalObject(irrobject, mass)
{
	IMeshBuffer* mbuffer = ((IAnimatedMeshSceneNode*)irrobject)->getMesh()->getMeshBuffer(0);

	int numVertices = mbuffer->getVertexCount();

	btVector3 vertexBase[numVertices];

	S3DVertex* vertices = (S3DVertex*)(mbuffer->getVertices());
	vector3df scale = irrobject->getScale();

	btConvexHullShape* chshape = new btConvexHullShape();

	for(int i = 0; i < numVertices; i++)
	{
		vertexBase[i].setX((float)(vertices[i].Pos.X * scale.X));
		vertexBase[i].setY((float)(vertices[i].Pos.Y * scale.Y));
		vertexBase[i].setZ((float)(vertices[i].Pos.Z * scale.Z));
		chshape->addPoint(vertexBase[i]);
	}

	initialize(chshape);

}

ConvexHull::~ConvexHull()
{

}
