#include "Terrain.h"

Terrain::Terrain(ISceneNode* irrterrain, IImage* heightfield, vector3df scale)
		:PhysicalObject(irrterrain, 0.f)
{
	//*
	float xScale = scale.X, yScale = scale.Y, zScale = scale.Z;

	int img_width = heightfield->getDimension().Width;
	int img_height = heightfield->getDimension().Height;

	f32* rawhf = new f32[img_width*img_height];

	int i = 0;
	f32 maxHeight = 0, minHeight = 0;

	for(int y = img_height - 1; y >= 0 ; y--)
	{
		for(int x = 0; x < img_width; x++)
		{
			rawhf[i] = (f32) (((heightfield->getPixel(x, y).color) << 24) >> 24);

			if(i == 0){
				minHeight = maxHeight = rawhf[0];
			}else{
				maxHeight = (rawhf[i] > maxHeight) ? rawhf[i] : maxHeight;
				minHeight = (rawhf[i] < minHeight) ? rawhf[i] : minHeight;
			}

			i++;
		}
	}

    btHeightfieldTerrainShape *pTerrain = new btHeightfieldTerrainShape ( img_width,
																		  img_height,
																		  rawhf,
																		  1,         //height scale
																		  minHeight, //min height
																		  maxHeight, //max height
																		  1,         //y-axis up
																		  PHY_FLOAT, //32-bit float heightfield
																		  false);

	// scale the shape (x and z)
	btVector3 localScaling = btVector3(xScale, yScale, zScale);
	pTerrain->setLocalScaling(localScaling);

	btTransform tr = btTransform();
	btQuaternion rot;
	rot.setRotation(btVector3(0,1,0), PI);
	tr.setIdentity();
	tr.setRotation(rot);
	tr.setOrigin(btVector3(0.0, (maxHeight - minHeight)*yScale/2.0, 0.0));

	// create ground object
	btDefaultMotionState* motionState = new btDefaultMotionState(tr);
	initializeTerrain(pTerrain, motionState);
    //*/
}

Terrain::~Terrain()
{
}

float Terrain::getHeight(float x, float y)
{
	return ((ITerrainSceneNode*)getSceneNode())->getHeight(x, y);
}
