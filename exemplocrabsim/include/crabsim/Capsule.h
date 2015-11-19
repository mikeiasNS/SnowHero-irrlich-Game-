#ifndef CAPSULE_H
#define CAPSULE_H

#include "PhysicalObject.h"

class Capsule : public PhysicalObject
{
	public:
		Capsule(ISceneNode *irrcapsule, double mass);
		~Capsule() {};
};

#endif // CAPSULE_H
