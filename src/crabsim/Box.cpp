#include "Box.h"

Box::Box(ISceneNode *irrbox, double mass)
	   :PhysicalObject(irrbox, mass)
{
	btVector3 size;
	vector3df scale = irrbox->getScale();

	size.setX(scale.X*(((IAnimatedMeshSceneNode*)irrbox)->getMesh()->getBoundingBox().MaxEdge.X - ((IAnimatedMeshSceneNode*)irrbox)->getMesh()->getBoundingBox().MinEdge.X)/2.0 );
	size.setY(scale.Y*(((IAnimatedMeshSceneNode*)irrbox)->getMesh()->getBoundingBox().MaxEdge.Y - ((IAnimatedMeshSceneNode*)irrbox)->getMesh()->getBoundingBox().MinEdge.Y)/2.0 );
	size.setZ(scale.Z*(((IAnimatedMeshSceneNode*)irrbox)->getMesh()->getBoundingBox().MaxEdge.Z - ((IAnimatedMeshSceneNode*)irrbox)->getMesh()->getBoundingBox().MinEdge.Z)/2.0 );

	initialize(new btBoxShape( size ));
}
