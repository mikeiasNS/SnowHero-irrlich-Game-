#include "BlenderImport.h"

btBlenderImporter::btBlenderImporter(vector3df pos) : btBulletWorldImporter(NULL)
{
	this->position.setX(pos.X);
	this->position.setY(pos.Y);
	this->position.setZ(pos.Z);
}

btBlenderImporter::~btBlenderImporter()
{

}

btRigidBody* btBlenderImporter::createRigidBody(bool isDynamic, btScalar mass, const btTransform& startTransform, btCollisionShape* shape, const char* bodyName)
{
	btVector3 localInertia;
	localInertia.setZero();

	if (mass)
		shape->calculateLocalInertia(mass,localInertia);

	btRigidBody* body = new btRigidBody(mass,0,shape,localInertia);

	btScalar y;
	btTransform trans;
	trans.setIdentity();

	btQuaternion q = startTransform.getRotation();
	y = q.z();
	q.setZ(-q.y());
	q.setY(-y);
	q.setX(-q.x());
	trans.setRotation(q);

	btVector3 pos = startTransform.getOrigin();
	y = pos.z();
	pos.setZ(pos.y());
	pos.setY(y);
	pos += this->position;
	trans.setOrigin(pos);

	body->setWorldTransform(trans);

	btVector3 ls = body->getCollisionShape()->getLocalScaling();
	y = ls.z();
	ls.setZ(ls.y());
	ls.setY(y);
	body->getCollisionShape()->setLocalScaling(ls);

	if (m_dynamicsWorld)
		m_dynamicsWorld->addRigidBody(body);

	if (bodyName)
	{
		char* newname = duplicateName(bodyName);
		m_objectNameMap.insert(body,newname);
		m_nameBodyMap.insert(newname,body);
	}

	m_allocatedRigidBodies.push_back(body);
	return body;

}

btCollisionShape* btBlenderImporter::createCylinderShapeZ(btScalar radius,btScalar height)
{
	btCylinderShape* shape = new btCylinderShape(btVector3(radius,height,radius));
	m_allocatedCollisionShapes.push_back(shape);
	return shape;
}

btCollisionShape* btBlenderImporter::createBoxShape(const btVector3& halfExtents)
{
	btVector3 he;
	he.setX(halfExtents.x());
	he.setY(halfExtents.z());
	he.setZ(halfExtents.y());

	btBoxShape* shape = new btBoxShape(he);
	m_allocatedCollisionShapes.push_back(shape);
	return shape;
}

btHingeConstraint* btBlenderImporter::createHingeConstraint(btRigidBody& rbA,btRigidBody& rbB, const btTransform& rbAFrame, const btTransform& rbBFrame, bool useReferenceFrameA)
{
	btTransform aFrame, bFrame;
	aFrame.setIdentity();
	bFrame.setIdentity();

	btVector3 originA, originB;
	btMatrix3x3 baseA, baseB;
	btQuaternion rotA, rotB;

	originA = rbAFrame.getOrigin();
	originB = rbBFrame.getOrigin();

	//Origin correction
	originA = btVector3(originA.x(), originA.z(), originA.y());
	originB = btVector3(originB.x(), originB.z(), originB.y());
	aFrame.setOrigin(originA);
	bFrame.setOrigin(originB);

	//Basis correction
	btMatrix3x3 baseo = btMatrix3x3(
									-1, 0, 0,
									0, 0, -1,
									0, -1, 0
									);
	baseA = baseo;
	baseB = baseo;

	//Rotation correction
	rotA = rbAFrame.getRotation();
	baseA *= btMatrix3x3(rotA);
	aFrame.setBasis(baseA);

	rotB = rbBFrame.getRotation();
	baseB *= btMatrix3x3(rotB);
	bFrame.setBasis(baseB);

	/* Debug
	cout << "Base B:\n";
	cout << "X = " << basB[0][0] << ", " << basB[0][1] << ", " << basB[0][2] << endl;
	cout << "Y = " << basB[1][0] << ", " << basB[1][1] << ", " << basB[1][2] << endl;
	cout << "Z = " << basB[2][0] << ", " << basB[2][1] << ", " << basB[2][2] << endl;

	SceneObject* axisx = engine->loadSceneObject("axisx.3ds");
	SceneObject* axisy = engine->loadSceneObject("axisy.3ds");
	SceneObject* axisz = engine->loadSceneObject("axisz.3ds");

	btVector3 ax = btVector3(1,0,0), ay = btVector3(0,1,0), az = btVector3(0,0,1);
	axisx->setPosition(vector3df(0, 20, 0));
	axisy->setPosition(vector3df(0, 20, 0));
	axisz->setPosition(vector3df(0, 20, 0));

	quaternion qax;
	qax.rotationFromTo(vector3df(ax.x(), ax.y(), ax.z()), vector3df(basB[0][0], basB[0][1], basB[0][2]));
	vector3df vax;
	qax.toEuler(vax);
	axisx->setRotation(vector3df(vax.X GRAD, vax.Y GRAD, vax.Z GRAD));

	quaternion qay;
	qay.rotationFromTo(vector3df(ay.x(), ay.y(), ay.z()), vector3df(basB[1][0], basB[1][1], basB[1][2]));
	vector3df vay;
	qay.toEuler(vay);
	axisy->setRotation(vector3df(vay.X GRAD, vay.Y GRAD, vay.Z GRAD));

	quaternion qaz;
	qaz.rotationFromTo(vector3df(az.x(), az.y(), az.z()), vector3df(basB[2][0], basB[2][1], basB[2][2]));
	vector3df vaz;
	qaz.toEuler(vaz);
	axisz->setRotation(vector3df(vaz.X GRAD, vaz.Y GRAD, vaz.Z GRAD));
*/

	btHingeConstraint* hinge = new btHingeConstraint(rbA,rbB,aFrame,bFrame,useReferenceFrameA);
	m_allocatedConstraints.push_back(hinge);

	return hinge;
}

btHingeConstraint* btBlenderImporter::createHingeConstraint(btRigidBody& rbA,const btTransform& rbAFrame, bool useReferenceFrameA)
{
	btVector3 origin;
	btQuaternion rot;

	btTransform aFrame = rbAFrame;
	origin = rbAFrame.getOrigin();
	rot = rbAFrame.getRotation();
	aFrame.setOrigin(btVector3(origin.x(), origin.z(), origin.y()));
	aFrame.setRotation(btQuaternion(rot.x(), rot.y(), rot.z(), rot.w()));

	btHingeConstraint* hinge = new btHingeConstraint(rbA,aFrame,useReferenceFrameA);
	m_allocatedConstraints.push_back(hinge);
	return hinge;
}
