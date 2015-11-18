#include "Sphere.h"

Sphere::Sphere(ISceneNode *irrsphere, double mass)
	   :PhysicalObject(irrsphere, mass)
{

	float radius = (((IAnimatedMeshSceneNode*)irrsphere)->getMesh()->getBoundingBox().MaxEdge.Y -
	                ((IAnimatedMeshSceneNode*)irrsphere)->getMesh()->getBoundingBox().MinEdge.Y)/2.0;

	radius = radius*(irrsphere->getScale().X);

	btSphereShape* pSphere = new btSphereShape( radius );

	initialize(pSphere);
}
