#include "PhysicalObject.h"


PhysicalObject::PhysicalObject(ISceneNode* irrnode, double mass, stringc name)
{
	this->mass = mass;
	this->irrnode = irrnode;
	this->name = name;

	this->trisel = this->irrnode->getSceneManager()->createTriangleSelectorFromBoundingBox(this->irrnode);
}

PhysicalObject::PhysicalObject(ISceneNode* irrnode, btRigidBody* body, stringc name)
{
	this->irrnode = irrnode;
	this->shape = body->getCollisionShape();
	this->rigidBody = body;

	btVector3 localScaling = rigidBody->getCollisionShape()->getLocalScaling();
	irrnode->setScale(vector3df(localScaling.x(), localScaling.y(), localScaling.z()));

	this->name = name;

	this->trisel = this->irrnode->getSceneManager()->createTriangleSelectorFromBoundingBox(this->irrnode);

	Update();
}

void PhysicalObject::initializeTerrain(btCollisionShape* shape, btDefaultMotionState* motionState)
{
	this->shape = shape;
	this->motionState = motionState;

    btVector3 inertia = btVector3(0,0,0);
    this->shape->calculateLocalInertia(0.0, inertia);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0.0, motionState, this->shape, inertia);
    this->rigidBody = new btRigidBody(rigidBodyCI);

    this->trisel = NULL;
}

void PhysicalObject::initialize(btCollisionShape* shape)
{
	vector3df pos, rotation;
	pos = irrnode->getPosition();
	rotation = irrnode->getRotation();
	btQuaternion rot;
	rot.setEuler(rotation.Y*PI/180.0, rotation.X*PI/180.0, rotation.Z*PI/180.0);

	this->shape = shape;

	btTransform trans;
	trans.setIdentity();

    trans.setRotation(rot);
    trans.setOrigin(btVector3(pos.X, pos.Y, pos.Z));

	this->motionState = new btDefaultMotionState(trans);
	btScalar m = this->mass;
    btVector3 inertia(0,0,0);
    this->shape->calculateLocalInertia(m, inertia);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(m, this->motionState, this->shape, inertia);
    this->rigidBody = new btRigidBody(rigidBodyCI);

    this->rigidBody->setCenterOfMassTransform(trans);

}

PhysicalObject::~PhysicalObject()
{
	irrnode->remove();
	if(trisel)
		trisel->drop();

	delete rigidBody->getMotionState();
    delete rigidBody;
	delete shape;
}

void PhysicalObject::setColor(Color clr)
{
	irrnode->setMaterialFlag(EMF_LIGHTING, true);
	irrnode->getMaterial(0).ColorMaterial = ECM_AMBIENT;
	irrnode->getMaterial(0).DiffuseColor = clr;
	irrnode->getMaterial(0).EmissiveColor = clr;
}

void PhysicalObject::translate(Vector3D vec)
{
	rigidBody->translate(btVector3(vec.X, vec.Y, vec.Z));
	Vector3D v = irrnode->getPosition();
	irrnode->setPosition(v + vec);
	this->Update();
}

void PhysicalObject::rotate(Vector3D vec, float ang){
    btQuaternion qt = btQuaternion(btVector3(vec.X, vec.Y, vec.Z), ang);
    btTransform tf = btTransform(qt);
    rigidBody->proceedToTransform(tf);
    this->Update();
}

void PhysicalObject::scale(Vector3D vec){
    btTransform tf = btTransform();
    tf.setIdentity();
    tf.setBasis(btMatrix3x3(vec.X, 0, 0,
                            0, vec.Y, 0,
                            0, 0, vec.Z));
    rigidBody->proceedToTransform(tf);

    irrnode->setScale(vec);
    this->Update();
}

void PhysicalObject::Update()
{
	rigidBody->activate();

	btVector3 pos = rigidBody->getCenterOfMassPosition();
	btQuaternion rot = rigidBody->getOrientation();

	btVector3 eulerRot;

    btScalar W = rot.getW();
	btScalar X = rot.getX();
	btScalar Y = rot.getY();
	btScalar Z = rot.getZ();
	float WSquared = W * W;
	float XSquared = X * X;
	float YSquared = Y * Y;
	float ZSquared = Z * Z;

	eulerRot.setX(atan2f(2.0f * (Y * Z + X * W), -XSquared - YSquared + ZSquared + WSquared));
	eulerRot.setY(asinf(-2.0f * (X * Z - Y * W)));
	eulerRot.setZ(atan2f(2.0f * (X * Y + Z * W), XSquared - YSquared - ZSquared + WSquared));

	eulerRot.setX( eulerRot.x()*180.0/PI );
	eulerRot.setY( eulerRot.y()*180.0/PI );
	eulerRot.setZ( eulerRot.z()*180.0/PI );

	if(irrnode != NULL)
	{
		irrnode->setPosition(vector3df(pos.x(), pos.y(), pos.z()));
		irrnode->setRotation(vector3df(eulerRot.x(), eulerRot.y(), eulerRot.z()));
	}
}

btRigidBody* PhysicalObject::getRigidBody()
{
	return rigidBody;
}

ISceneNode* PhysicalObject::getSceneNode()
{
	return irrnode;
}

ITriangleSelector* PhysicalObject::getTriangleSelector()
{
	return trisel;
}

MeshBuffer* PhysicalObject::getMeshBuffer()
{
	return ((IAnimatedMeshSceneNode*)irrnode)->getMesh()->getMesh(0)->getMeshBuffer(0);
}

Vector3D PhysicalObject::getScale()
{
	return irrnode->getScale();
}

Vector3D PhysicalObject::getPosition()
{
	return irrnode->getPosition();
}

Vector3D PhysicalObject::getRotation()
{
	return irrnode->getRotation();
}

quaternion PhysicalObject::getOrientation()
{
	btQuaternion rot = rigidBody->getOrientation();

	return quaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW());
}

matrix4 PhysicalObject::getAbsoluteTransformation()
{
	return irrnode->getAbsoluteTransformation();
}

Vector3D PhysicalObject::getBoundingBoxSize()
{
    aabbox3df box = irrnode->getBoundingBox();
    vector3df min, max;
    min = box.MinEdge;
    max = box.MaxEdge;

    vector3df scale = irrnode->getScale();

	return vector3df((max.X - min.X)*scale.X, (max.Y - min.Y)*scale.Y, (max.Z - min.Z)*scale.Z);
}

BoundingBox PhysicalObject::getBoundingBox()
{
	return irrnode->getBoundingBox();
	//return bbox;
}

BoundingBox PhysicalObject::getTransformedBoundingBox()
{
	//return irrnode->getTransformedBoundingBox();
//	return bbox;
	//*
	btVector3 min, max;
	rigidBody->getAabb(min, max);

	Vector3D bmin(min.x(), min.y(), min.z()), bmax(max.x(), max.y(), max.z());

	return aabbox3df(bmin, bmax);
	//*/
}

bool PhysicalObject::intersectBoundingBox(PhysicalObject* other)
{
	aabbox3df box = other->getBoundingBox();
	return irrnode->getBoundingBox().intersectsWithBox(box);
}

Vector3D PhysicalObject::getCenterOfMass()
{
    btVector3 cog = rigidBody->getCenterOfMassPosition();

    return Vector3D(cog.x(), cog.y(), cog.z());
}

float PhysicalObject::getMass()
{
    return 1.0/rigidBody->getInvMass();
}

Vector3D PhysicalObject::getLocalBasisAxis(int axis, quaternion basisRotation)
{
    vector3df base = vector3df(1,0,0);
	if(axis == 1) base = vector3df(0,1,0);
	else if(axis == 2) base = vector3df(0,0,1);

	base = basisRotation * base;

	btQuaternion btq = rigidBody->getOrientation();
	quaternion q = quaternion(btq.x(), btq.y(), btq.z(), btq.w());

	base = q * base;

	return base;
}

float PhysicalObject::getAngularVel()
{
	btVector3 vel =	rigidBody->getAngularVelocity();

	return vel.length();
}

Vector3D PhysicalObject::getLinearVel()
{
	btVector3 vel =	rigidBody->getLinearVelocity();

	return Vector3D(vel.x(), vel.y(), vel.z());
}

void PhysicalObject::setRestitution(float c)
{
	rigidBody->setRestitution(c);
}

void PhysicalObject::setFriction(float c)
{
	rigidBody->setFriction(c);
}

void PhysicalObject::setAngularDamping(float damp)
{
	btScalar lin = rigidBody->getLinearDamping();
	rigidBody->setDamping(lin, damp);
}

void PhysicalObject::setLinearDamping(float damp)
{
	btScalar ang = rigidBody->getAngularDamping();
	rigidBody->setDamping(damp, ang);
}

stringc PhysicalObject::getName()
{
	return this->name;
}

void PhysicalObject::setLinearVelocity(Vector3D value)
{
	rigidBody->setLinearVelocity(btVector3(value.X, value.Y, value.Z));
}

void PhysicalObject::setAngularVelocity(Vector3D value)
{
	rigidBody->setAngularVelocity(btVector3(value.X, value.Y, value.Z));
}

void PhysicalObject::viewWireframe(bool wire)
{
	this->irrnode->setMaterialFlag(EMF_WIREFRAME, wire);
}

void PhysicalObject::lock2D()
{
	rigidBody->setLinearFactor(btVector3(1,0,1));
    //rigidBody->setAngularFactor(btVector3(0,1,0));
}

void PhysicalObject::lock3D()
{
	rigidBody->setLinearFactor(btVector3(0,0,0));
    rigidBody->setAngularFactor(btVector3(0,0,0));
}
