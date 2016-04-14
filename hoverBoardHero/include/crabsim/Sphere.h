#ifndef SPHERE_H
#define SPHERE_H

#include "PhysicalObject.h"

class Sphere : public PhysicalObject
{
	public:
		Sphere(ISceneNode *irrsphere, double mass);
		~Sphere() {};
};

#endif // SPHERE_H
