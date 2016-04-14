#ifndef PHYSICALOBJECT_H
#define PHYSICALOBJECT_H

#include <cmath>
#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>
#include "Joint.h"

#define MeshBuffer IMeshBuffer
#define Vector3D vector3df
#define Transform btTransform
#define BoundingBox aabbox3df
#define Color SColor

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

enum Shape
{
	shSphere, shBox, shCapsule, shCylinder, shCone, shConvexHull, shTriMesh
};

class PhysicalObject
{
	friend class PhysicsSim;
	friend class Joint;

	private:
		btRigidBody* rigidBody;
		btCollisionShape* shape;
		btDefaultMotionState* motionState;
		double mass;

        ISceneNode* irrnode;
        ITriangleSelector* trisel;

        stringc name;

	protected:
		void initializeTerrain(btCollisionShape* shape, btDefaultMotionState* motionState);
        void initialize(btCollisionShape* shape);

		void Update();

		ISceneNode* getSceneNode();
		btVector3 getJointReferenceAxis();

	public:

		PhysicalObject(ISceneNode* irrnode, btRigidBody* body, stringc name = L"");
		PhysicalObject(ISceneNode* irrnode, double mass, stringc name = L"");
		~PhysicalObject();

		void setColor(Color clr);

        btRigidBody* getRigidBody();
		ITriangleSelector* getTriangleSelector();
		MeshBuffer* getMeshBuffer();
		Vector3D getScale();
		Vector3D getPosition();
		Vector3D getRotation();
		quaternion getOrientation();

		void translate(Vector3D vec);
		void rotate(Vector3D vec, float ang);
		void scale(Vector3D vec);
		matrix4 getAbsoluteTransformation();
		Vector3D getBoundingBoxSize();
		BoundingBox getBoundingBox();
		BoundingBox getTransformedBoundingBox();
		bool intersectBoundingBox(PhysicalObject* other);
		Vector3D getCenterOfMass();
		float getMass();

		Vector3D getLocalBasisAxis(int axis, quaternion basisRotation = quaternion(0,0,0));

		float getAngularVel();
		Vector3D getLinearVel();

		void setRestitution(float c);
		void setFriction(float c);

		void setLinearDamping(float damp);
		void setAngularDamping(float damp);

		stringc getName();

		void setLinearVelocity(Vector3D value);
		void setAngularVelocity(Vector3D value);

		void viewWireframe(bool wire = true);

		void lock2D();
		void lock3D();

};

#endif // PHYSICALOBJECT_H
