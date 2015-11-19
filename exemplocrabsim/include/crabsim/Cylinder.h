#ifndef CYLINDER_H
#define CYLINDER_H

#include "PhysicalObject.h"

class Cylinder : public PhysicalObject
{
	public:
		Cylinder(ISceneNode *irrcylinder, double mass);
		~Cylinder() {};
};


#endif // CYLINDER_H
