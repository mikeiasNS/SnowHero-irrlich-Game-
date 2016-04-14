#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include "PhysicalObject.h"

class ConvexHull : public PhysicalObject
{
	public:
		ConvexHull(ISceneNode *irrobject, double mass);
		~ConvexHull();
};

#endif // CONVEXHULL_H
