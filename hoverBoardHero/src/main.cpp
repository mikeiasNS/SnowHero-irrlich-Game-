#include "CrabSim.h"
#include <iostream>

bool menu = true;
bool creditos = false;
Vector3D heroPos;
btTransform heroBase;
PhysicalStructure percurso;
PhysicalObject* boneco;
SceneObject* malhaP;
PhysicalObject* tPercurso;
btTransform q;
Camera* fpscam;
btVector3 zAxis;
btVector3 tAxis;

void start(){
    btTransform tT = boneco->getRigidBody()->getCenterOfMassTransform();
    btMatrix3x3 tMat = tT.getBasis();
    tAxis = tMat.getColumn(2);
}


void turnLeft(){
//    boneco->getRigidBody()->applyCentralForce(tAxis);
    boneco->getRigidBody()->setAngularVelocity(btVector3(0, -1, 0));
//    tPercurso->getRigidBody()->setAngularVelocity(btVector3(-1, 0, 0));
}

void turnRight(){
//    boneco->getRigidBody()->applyCentralForce(-(tAxis));
    boneco->getRigidBody()->setAngularVelocity(btVector3(0, 1, 0));
//    tPercurso->getRigidBody()->setAngularVelocity(btVector3(1, 0, 0));
}

void restart(){
    menu = false;
    creditos = false;
    q = btTransform(heroBase.getBasis());
    boneco->getRigidBody()->proceedToTransform(heroBase);

    boneco->getRigidBody()->clearForces();
    boneco->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
    boneco->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
}

static bool eventFunction(PhysicsSim *engine, const SEvent &event){

	if (event.EventType == EET_KEY_INPUT_EVENT){
        switch( event.KeyInput.Key ){
            case KEY_KEY_M:
                menu = true;
                break;
            case KEY_KEY_A:
                turnLeft();
                break;
            case KEY_KEY_D:
                turnRight();
                break;
            case KEY_KEY_R:
                restart();

                if(event.KeyInput.PressedDown){
                    engine->pause();
                }
                break;
            case KEY_KEY_C:
                creditos = true;
                break;
            case KEY_SPACE:
                boneco->getRigidBody()->setAngularVelocity(btVector3(0,0,0));
                break;
            case KEY_KEY_P:
                boneco->getRigidBody()->applyCentralForce(zAxis * 800);
                break;
            case KEY_KEY_X:
                Vector3D pp = boneco->getRotation();
                cout << pp.X << ", " << pp.Y << ", " << pp.Z << endl;
                break;
        }
	}

	return false;
}

int main(int argn, char* args[])
{
    PhysicsSim engine("HoverBoard Hero", 1366, 1024);

	engine.registerListener(eventFunction);
	engine.setMediaDirectory(".\\media\\");
	engine.setWindowTitle("HoverBoard Hero");

	fpscam = engine.addCamera();

	engine.makeLight(vector3df(0, 1000, 500), 8000.0f);
	engine.addSky("sky7.jpg");

    SceneObject* malha = engine.loadSceneObject("course2\\martymcfly\\marty.dae", "course2\\martymcfly\\marty.jpg");
    malha->setRotation(Vector3D(160,-2,-160));
    boneco = engine.applyPhysics(malha, shConvexHull, 0.1);

    malhaP = engine.loadSceneObject("course2\\GPT.dae", "course2\\metal.jpg");
    malhaP->setRotation(Vector3D(-90,2,180));
    tPercurso = engine.applyPhysics(malhaP, shTriMesh, 1.0);

    tPercurso->lock3D();

    boneco->translate(Vector3D(-450, -40, 0));
    start();

    Vector3D vec = Vector3D(0, 0, 1);
    heroBase = boneco->getRigidBody()->getCenterOfMassTransform();
    engine.setGravity(40);

    boneco->setFriction(0);
	while(engine.run()){
        if(menu){
            engine.drawImage("hoverboardhero.jpg");
        } else if (creditos){
            engine.drawImage("credits.jpg");
        } else {
            btTransform t = boneco->getRigidBody()->getCenterOfMassTransform();
            btMatrix3x3 mat = t.getBasis();
            zAxis = mat.getColumn(0);
            vec = Vector3D(zAxis.x(), zAxis.y(), zAxis.z());
            heroPos = boneco->getPosition();

            fpscam->setPosition((heroPos - 20 * vec) + Vector3D(10,10,0));
            fpscam->setTarget(heroPos);
        }
        engine.view(fpscam);
	}

    return 0;
}
