#ifndef BLENDERIMPORT_H
#define BLENDERIMPORT_H

#include "PhysicsSim.h"
#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletWorldImporter/btBulletWorldImporter.h>

using namespace irr;
using namespace core;

class PhysicsSim;

class btBlenderImporter : public btBulletWorldImporter
{
	private:
		btVector3 position;

	public:
		btBlenderImporter(vector3df pos);
		~btBlenderImporter();

		btRigidBody* createRigidBody(bool isDynamic, btScalar mass, const btTransform& startTransform, btCollisionShape* shape, const char* bodyName);
		btCollisionShape* createCylinderShapeZ(btScalar radius,btScalar height);
		btCollisionShape* createBoxShape(const btVector3& halfExtents);

		btHingeConstraint* createHingeConstraint(btRigidBody& rbA,btRigidBody& rbB, const btTransform& rbAFrame, const btTransform& rbBFrame, bool useReferenceFrameA);
		btHingeConstraint* createHingeConstraint(btRigidBody& rbA,const btTransform& rbAFrame, bool useReferenceFrameA);
};

#endif // BLENDERIMPORT_H
