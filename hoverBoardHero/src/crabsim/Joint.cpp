#include "Joint.h"

Joint::Joint(PhysicalObject* obj1, PhysicalObject* obj2, vector3df anchor, vector3df axis = vector3df(0, 0, 1))
{
	type = JTHinge;
	constraint = createHinge(obj1, obj2, btVector3(anchor.X, anchor.Y, anchor.Z), btVector3(axis.X, axis.Y, axis.Z));
}

Joint::Joint(btTypedConstraint* constraint)
{
	type = JTHinge;
	this->constraint = constraint;
}

Joint::~Joint()
{
	delete constraint;
}

JointType Joint::getType()
{
	return type;
}

btTypedConstraint* Joint::getConstraint()
{
	return constraint;
}

btHingeConstraint* Joint::createHinge(PhysicalObject* obj1, PhysicalObject* obj2, btVector3 anchor, btVector3 axis)
{
   btVector3 anchorInA, anchorInB;

   anchorInA = anchor;

   anchorInB = obj2->getRigidBody()->getCenterOfMassTransform().inverse() * (obj1->getRigidBody()->getCenterOfMassTransform() * anchor);

   return new btHingeConstraint (*(obj1->getRigidBody()), *(obj2->getRigidBody()),
								 anchorInA, anchorInB,
								 axis,
								 axis,
								 true);

}

void Joint::setLimits(float min, float max)
{
	if(type == JTHinge)
	{
		btScalar radMin = min*PI/180.0,
				 radMax = max*PI/180.0;

		//setLimit (btScalar low, btScalar high, btScalar _softness=0.9f, btScalar _biasFactor=0.3f, btScalar _relaxationFactor=1.0f)
		((btHingeConstraint*)constraint)->setLimit (radMin, radMax);
	}
}

void Joint::getLimits(float &min, float &max)
{

	min = ((btHingeConstraint*)constraint)->getLowerLimit();
	max = ((btHingeConstraint*)constraint)->getUpperLimit();

}

float Joint::getAngle()
{
	return ((btHingeConstraint*)constraint)->getHingeAngle();
}

void Joint::move(float force)
{
	if(type == JTHinge)
	{
		if(force == 0)
		{
			((btHingeConstraint*)constraint)->enableMotor(false);
			return;
		}
		((btHingeConstraint*)constraint)->enableMotor(true);
		//((btHingeConstraint*)constraint)->setMotorTarget(force, 0.00001);

		//force = (force > ((btHingeConstraint*)constraint)->getMaxMotorImpulse()) ? ((btHingeConstraint*)constraint)->getMaxMotorImpulse() : force;

		if(force > 0)
			((btHingeConstraint*)constraint)->enableAngularMotor(true, force, force);
		else
			((btHingeConstraint*)constraint)->enableAngularMotor(true, force, -force);

	}
}
