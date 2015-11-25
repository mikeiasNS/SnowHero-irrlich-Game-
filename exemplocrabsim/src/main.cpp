#include "CrabSim.h"
#include <iostream>

bool menu = false;
bool creditos = false;
int directionCounter = 0;
Vector3D heroPos;
btTransform heroBase;
PhysicalStructure percurso;
PhysicalObject* marty;
PhysicalObject* boneco;
btTransform q;
Camera* fpscam;

void turnLeft(){
    boneco->getRigidBody()->setAngularVelocity(btVector3(0, -1, 0));
    //boneco->getRigidBody()->applyCentralForce(btVector3(10, 0, 0));
}

void turnRight(){
    boneco->getRigidBody()->setAngularVelocity(btVector3(0, 1, 0));
    //boneco->getRigidBody()->applyCentralForce(btVector3(-10, 0, 0));
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
            case KEY_SPACE:
                boneco->getRigidBody()->applyCentralImpulse(btVector3(0, 2, 0));
                break;
            case KEY_KEY_R:
                menu = false;
                creditos = false;
                q = btTransform(heroBase.getBasis());
                boneco->getRigidBody()->proceedToTransform(heroBase);
                if(event.KeyInput.PressedDown){
                    engine->pause();
                }
                boneco->getRigidBody()->clearForces();
                boneco->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
                boneco->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
                break;
            case KEY_KEY_C:
                creditos = true;
                break;
            case KEY_KEY_X:
                cout << "Boneco: " << boneco->getPosition().X << ", " << boneco->getPosition().Y << ", " << boneco->getPosition().Z << endl;
                break;
        }
	}

	return false;
}

int main(int argn, char* args[])
{
    PhysicsSim engine("SnowHero", 1366, 768);

	engine.registerListener(eventFunction);
	engine.setMediaDirectory(".\\media\\");
	engine.setWindowTitle("Teste");

	fpscam = engine.addCamera();

	engine.makeLight(vector3df(0, 1000, 500), 8000.0f);
	engine.addSky("sky7.jpg");

    SceneObject* malha = engine.loadSceneObject("course2\\martymcfly\\marty.dae", "course2\\martymcfly\\marty.jpg");
    malha->setRotation(Vector3D(-15,90,-20));
    boneco = engine.applyPhysics(malha, shTriMesh, 0.1);

    engine.loadPhysicalObjects(percurso, "course2\\MyCourse2.dae");

    PhysicalStructure tree;
    engine.loadPhysicalObjects(tree, "course2\\tree.dae");

//    tree.bodies[0]->lock3D();
//
//    tree.bodies[0]->translate(Vector3D(20, -3, -40));
//
//    PhysicalStructure tree1;
//    engine.loadPhysicalObjects(tree1, "course2\\tree.dae");
//
//    tree1.bodies[0]->lock3D();
//
//    tree1.bodies[0]->translate(Vector3D(-20, -3, -40));

    boneco->translate(Vector3D(0, 10, 0));

    for(int i = 0; i < percurso.bodies.size(); i++){
            percurso.bodies[i]->lock3D();
            engine.setTexture(percurso.bodies[i], "grass.jpg");
    }

    Vector3D vec = Vector3D(0, 0, 1);
    heroBase = boneco->getRigidBody()->getCenterOfMassTransform();
    engine.setGravity(30);

    boneco->setFriction(0);
	while(engine.run()){
        if(menu){
            engine.drawImage("hoverboardhero.jpg");
        } else if (creditos){
            engine.drawImage("credits.jpg");
        } else {
            Vector3D mR = malha->getRotation();
            btTransform t = boneco->getRigidBody()->getCenterOfMassTransform();
            btMatrix3x3 mat = t.getBasis();
            btVector3 zAxis = mat.getColumn(0);
            vec = Vector3D(zAxis.x(), zAxis.y(), zAxis.z());
            heroPos = boneco->getPosition();
//            boneco->getRigidBody()->applyCentralForce((zAxis));
            fpscam->setPosition((heroPos - 20 * vec) + Vector3D(0,10,0));
            fpscam->setTarget(heroPos);
        }
        engine.view(fpscam);
	}

    return 0;
}
