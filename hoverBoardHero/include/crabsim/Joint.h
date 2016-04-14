#ifndef JOINT_H
#define JOINT_H

#include "PhysicalObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class PhysicalObject;

enum JointType
{
	JTHinge, JTUniversal, JTBall, JTFixed
};

class Joint
{
	friend class PhysicsSim;

	protected:
		btTypedConstraint* getConstraint();

	private:
		JointType type;

		btTypedConstraint* constraint;

		//anchor: anchor position relative to obj1 basis
		btHingeConstraint* createHinge(PhysicalObject* obj1, PhysicalObject* obj2, btVector3 anchor, btVector3 axis);

	public:
		Joint(btTypedConstraint* constraint);
		Joint(PhysicalObject* obj1, PhysicalObject* obj2, vector3df anchor, vector3df axis);
		~Joint();

		JointType getType();

		//Hinge Joint
		void setLimits(float min, float max);
		void getLimits(float &min, float &max);
		float getAngle();
		void move(float force);

};

#endif // JOINT_H
