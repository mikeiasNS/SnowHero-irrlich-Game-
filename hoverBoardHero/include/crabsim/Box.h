#ifndef BOX_H
#define BOX_H

#include "PhysicalObject.h"

class Box : public PhysicalObject
{
	public:
		Box(ISceneNode *irrbox, double mass);
		~Box() {};
};

#endif // BOX_H
