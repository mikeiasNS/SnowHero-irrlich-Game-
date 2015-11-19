#ifndef CONE_H
#define CONE_H

#include "PhysicalObject.h"

class Cone : public PhysicalObject
{
	public:
		Cone(ISceneNode *irrcone, double mass);
		~Cone() {};
};

#endif // CONE_H
