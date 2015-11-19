#include "Capsule.h"

Capsule::Capsule(ISceneNode *irrcapsule, double mass)
	   :PhysicalObject(irrcapsule, mass)
{
	float radius, height;
	vector3df scale = irrcapsule->getScale();

	vector3df min = ((IAnimatedMeshSceneNode*)irrcapsule)->getMesh()->getBoundingBox().MinEdge;
	vector3df max = ((IAnimatedMeshSceneNode*)irrcapsule)->getMesh()->getBoundingBox().MaxEdge;

	radius = ((max.X - min.X)/2.0)*scale.X;
	height = (max.Y - min.Y)*scale.Y;

	initialize(new btCapsuleShape(radius, height - 2.0*radius));
}
