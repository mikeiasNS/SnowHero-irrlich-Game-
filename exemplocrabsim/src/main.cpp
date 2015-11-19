#include "CrabSim.h"
#include <iostream>

using namespace std;

bool turningLeft = false;
bool turningRight = false;
int directionCounter = 0;
Vector3D heroPos;
PhysicalStructure percurso;
PhysicalObject* boneco;
Camera* fpscam;

// dir: angle: 4.23685
// reto: angle: 3.9981
// esq: angle: 3.75935
void checkAngle(){
    btScalar ag = boneco->getRigidBody()->getOrientation().getAngle();
    if(turningLeft){
        if(ag < 4.23685 && ag > 3.75935){
            boneco->getRigidBody()->setAngularVelocity(btVector3(0, -1, 0));
        } else{
            boneco->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
        }
    } else if(turningRight){
        if(ag < 4.23685 && ag > 3.75935){
            boneco->getRigidBody()->setAngularVelocity(btVector3(0, 1, 0));
        } else{
            boneco->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
        }
    }
}

void turnLeft(){
    turningLeft = true;
    turningRight = false;
    boneco->getRigidBody()->setAngularVelocity(btVector3(0, -1, 0));
    boneco->getRigidBody()->applyCentralForce(btVector3(10, 0, 0));
}

void turnRight(){
    turningRight = true;
    turningLeft = false;
    boneco->getRigidBody()->setAngularVelocity(btVector3(0, 1, 0));
    boneco->getRigidBody()->applyCentralForce(btVector3(-10, 0, 0));
}

static bool eventFunction(PhysicsSim *engine, const SEvent &event){

	if (event.EventType == EET_KEY_INPUT_EVENT){
        switch( event.KeyInput.Key ){
            case KEY_KEY_W:
                //snowboard.bodies[0]->getRigidBody()->setLinearVelocity(btVector3(0, 0, 20));
                break;
            case KEY_KEY_S:
                //snowboard.bodies[0]->getRigidBody()->setLinearVelocity(btVector3(0, 0, -20));
                break;
            case KEY_KEY_A:
                turnLeft();
                break;
            case KEY_KEY_D:
                turnRight();
                break;
            case KEY_SPACE:
                boneco->getRigidBody()->applyCentralImpulse(btVector3(0, 2, 0));
                break;
            case KEY_KEY_R:
                heroPos = boneco->getPosition();
                boneco->translate(Vector3D(-heroPos.X, -heroPos.Y + 1, -heroPos.Z));
                break;
            case KEY_KEY_X:
                cout << "Boneco: " << boneco->getPosition().X << ", " << boneco->getPosition().Y << ", " << boneco->getPosition().Z << endl;
                cout << "Camera P: " << fpscam->getPosition().X << ", " << fpscam->getPosition().Y << ", " << fpscam->getPosition().Z << endl;
                cout << "Camera T: " << fpscam->getTarget().X << ", " << fpscam->getTarget().Y << ", " << fpscam->getTarget().Z << endl;
                break;
        }
	}

	return false;
}

/*void runGame(){
    btTransform t = boneco->getRigidBody()->getCenterOfMassTransform();
    btMatrix3x3 mat = t.getBasis();
    btVector3 zAxis = mat.getColumn(0);

    vec = Vector3D(zAxis.x(), zAxis.y() + 1, zAxis.z());
    boneco->getRigidBody()->applyCentralForce(-(zAxis * 1.2));
    Vector3D macPosition = boneco->getPosition();
    fpscam->setPosition(macPosition + vec);
    fpscam->setTarget(macPosition);

    engine.view(fpscam);
}*/

int main(int argn, char* args[])
{
    PhysicsSim engine;

	engine.registerListener(eventFunction);
	engine.setMediaDirectory(".\\media\\");
	engine.setWindowTitle("Teste");

	fpscam = engine.addFPSCamera();
	fpscam->setPosition(Vector3D(-10, 10, 0));

	engine.makeLight(vector3df(7000, 10000, -5000), 8000.0f);
	engine.addSky("sky6.jpg");

    engine.loadPhysicalObjects(percurso, "course2\\MyCourse2.dae");

    for(int i = 0; i < percurso.bodies.size(); i++){
        if(percurso.bodies[i]->getName() != "head"){
            percurso.bodies[i]->lock3D();
        } else{
            boneco = percurso.bodies[i];
        }
    }

    Vector3D vec = Vector3D(0, 0, 1);
    engine.setGravity(2);

	while(engine.run()){
        btTransform t = boneco->getRigidBody()->getCenterOfMassTransform();
        btMatrix3x3 mat = t.getBasis();
        btVector3 zAxis = mat.getColumn(0);

        vec = Vector3D(zAxis.x(), zAxis.y() + 1, zAxis.z());
        boneco->getRigidBody()->applyCentralForce(-(zAxis * 1.2));
        heroPos = boneco->getPosition();
        fpscam->setPosition(heroPos + vec);
        fpscam->setTarget(heroPos);

        engine.view(fpscam);
	}

    return 0;
}
