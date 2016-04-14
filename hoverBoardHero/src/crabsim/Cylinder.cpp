#include "Cylinder.h"

Cylinder::Cylinder(ISceneNode *irrcylinder, double mass)
	   :PhysicalObject(irrcylinder, mass)
{
	float radiusx, radiusz, height;
	vector3df scale = irrcylinder->getScale();

	vector3df min = ((IAnimatedMeshSceneNode*)irrcylinder)->getMesh()->getBoundingBox().MinEdge;
	vector3df max = ((IAnimatedMeshSceneNode*)irrcylinder)->getMesh()->getBoundingBox().MaxEdge;

	radiusx = ((max.X - min.X)/2.0)*scale.X;
	radiusz = ((max.Z - min.Z)/2.0)*scale.Z;
	height = (max.Y - min.Y)*scale.Y;

	initialize(new btCylinderShape(btVector3(radiusx, height/2.0, radiusz)));
}

