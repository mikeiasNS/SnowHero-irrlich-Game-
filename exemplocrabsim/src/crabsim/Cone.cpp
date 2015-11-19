#include "Cone.h"

Cone::Cone(ISceneNode *irrcone, double mass)
	   :PhysicalObject(irrcone, mass)
{
	float radius, height;
	vector3df scale = irrcone->getScale();

	vector3df min = ((IAnimatedMeshSceneNode*)irrcone)->getMesh()->getBoundingBox().MinEdge;
	vector3df max = ((IAnimatedMeshSceneNode*)irrcone)->getMesh()->getBoundingBox().MaxEdge;

	radius = ((max.X - min.X)/2.0)*scale.X;
	height = (max.Y - min.Y)*scale.Y;

	initialize(new btConeShape(radius, height));
}

