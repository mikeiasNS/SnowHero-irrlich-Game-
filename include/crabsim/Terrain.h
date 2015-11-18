#ifndef TERRAIN_H
#define TERRAIN_H

#include "PhysicalObject.h"
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

class Terrain : public PhysicalObject
{
	public:
		Terrain(ISceneNode* irrterrain, IImage* heightfield, vector3df scale);
		~Terrain();

		float getHeight(float x, float y);
};

#endif // TERRAIN_H
