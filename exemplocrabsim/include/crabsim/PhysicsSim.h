#ifndef PHYSICSSIM_H
#define PHYSICSSIM_H

#include <vector>
#include <iostream>

#include <irrlicht.h>
#include <irrString.h>
#include <btBulletDynamicsCommon.h>

#include <BulletCollision/Gimpact/btGImpactShape.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#define Camera ICameraSceneNode
#define SceneObject ISceneNode
#define Vector3D vector3df
#define Vector2D vector2df
#define Quaternion quaternion
#define Mesh IAnimatedMesh
#define MeshBuffer IMeshBuffer
#define Texture ITexture
#define Dimension dimension2du
#define Position position2di
#define Rectangle recti
#define Timer ITimer
#define Color SColor
#define GUIEnvironment IGUIEnvironment
#define Texture ITexture

#include "Joint.h"
#include "Sphere.h"
#include "Capsule.h"
#include "Box.h"
#include "Cylinder.h"
#include "Cone.h"
#include "TriMesh.h"
#include "ConvexHull.h"
#include "Terrain.h"
#include "BlenderImport.h"
#include <util.h>

#include <iostream>
#include <sstream>

using namespace std;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

struct VBox
{
	VBox(aabbox3df box, Color color) : box(box), color(color) {}
	aabbox3df box;
	Color color;
};

struct Line3D
{
	Line3D(Vector3D p1, Vector3D p2, Color color = Color(255,0,0,0)) : p1(p1), p2(p2), color(color) {}
	Vector3D p1, p2;
	Color color;
};

struct TextureInfo
{
	stringw name;
	position2di position;
};

struct Text
{
	stringw text;
	Position position;
	Color color;
	uint size;
};

class PhysicalStructure
{
	public:
		PhysicalStructure();
		PhysicalStructure(const PhysicalStructure &other);
		~PhysicalStructure();

		PhysicalStructure& operator=(const PhysicalStructure &other);

		vector<PhysicalObject*> bodies;
		vector<Joint*> joints;
		PhysicalObject* getBody(stringw name);
};

class PhysicsSim;
typedef bool (*ListenerFunction)(PhysicsSim *engine, const SEvent &);

class RenderTarget
{
	friend class PhysicsSim;

	private:
		ISceneManager* smgr;
		Texture* target;

	protected:
		Camera* camera;
		Dimension resolution;
		IVideoDriver* driver;

		void init(IrrlichtDevice* dvc);
		void drawAll();
		virtual void draw() = 0;

	public:
		RenderTarget(Camera* camera, Dimension resolution);
		RenderTarget();

		Camera* getCamera();
		Texture* getImage();
		void getImageData(floatv &data);

};

class PhysicsSim : public IEventReceiver
{

    private:

		//Bullet vars
        btBroadphaseInterface* broadphase;
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;

        btClock clock;

        //irrlicht vars
        IrrlichtDevice *dvc;
		IVideoDriver* driver;
		ISceneManager* smgr;
		IGUIEnvironment* env;
		IGUIImage* pipImage;

		bool KeyIsDown[KEY_KEY_CODES_COUNT];
		bool OnEvent(const SEvent& event);
		const bool IsKeyDown(EKEY_CODE keyCode);

		bool (*listener)(PhysicsSim *engine, const SEvent&);

		u32 lasttick;
		u32 ticks();
		bool paused;

		void updatePhysics();
		void updateObjects();
		void step();

		SceneObject *testSphere;
		PhysicalObject* terrain;

		Camera* fpscam;

		vector<PhysicalObject*> objects_list;
		vector<Joint*> joints_list;
		vector<RenderTarget*> render_targets;

		stringw mediaDirectory;

		IGUIFont *font;
		vector<Text> texts;
		vector<TextureInfo> images;
		vector<VBox> boxes;
		vector<IAnimatedMeshSceneNode*> arrows;
		vector<Line3D> lines;
		void drawLine(Vector3D pos, Vector3D vec, Color c = Color(255, 255, 255, 255));

    public:
        void setTexture(PhysicalObject* object, stringw texture_file);
        PhysicsSim(stringw windowTitle = "CRAb Physics Simulator", uint width = 640, uint height = 480, bool render = true);
        ~PhysicsSim();

        Timer* getTimer();

        void setWindowTitle(stringw windowTitle);
        Dimension getScreenSize();

        void setGravity(double gravity);

		void shootTestSphere();
		void drawVector(Vector3D init, Vector3D end, Color c = Color(255, 255, 255, 255));
		void drawLine(Line3D line);
		void drawBox(aabbox3df box, Color color = Color(255, 255, 255, 255));
		void drawImage(stringw filename, Position pos = Position(0,0));
		void drawText(stringw text, Position pos = Position(0,0), uint size = 2, Color color = Color(255,0,0,0));

		Terrain* getTerrain();

		void registerListener(ListenerFunction f);
		bool run();
		void pause();
		void view(Camera* cam, RenderTarget* pipTarget = NULL);

		void setMediaDirectory(stringw dir);
		bool existFile(stringw filename);

		Camera* addFPSCamera(Vector3D position = Vector3D(0, 1.0, -5), Vector3D target = Vector3D(0, 1.0, 0));
		Camera* addCamera();

		RenderTarget* addRenderTarget(RenderTarget* renderTarget);
		void removeRenderTarget(RenderTarget* rendertarget);

		void showPip(bool show);
		GUIEnvironment* getGUIEnvironment();

		void addTerrain(stringw heightfield_file, stringw texture_file, Vector3D scale = Vector3D(1.0, 0.5, 1.0));
		void addSky(stringw skydome_file);
		void makeLight(vector3df position, float intensity);

		SceneObject* loadSceneObject(stringw mesh_file, stringw texture_file = L"");
		PhysicalObject* applyPhysics(SceneObject* object, Shape shape, float mass);

		bool loadPhysicalObjects(stringc mesh_filename, Vector3D position = Vector3D(0, 0, 0), bool lock2d = false);
		bool loadPhysicalObjects(PhysicalStructure &structure, stringc mesh_filename, Vector3D position = Vector3D(0, 0, 0), bool lock2d = false);

		void removeObject(SceneObject* object);
		void removeObject(PhysicalObject* object);

		Joint* createJoint(PhysicalObject* obj1, PhysicalObject* obj2, Vector3D anchor, Vector3D axis);
		void removeJoint(Joint* joint);

		bool isColliding(PhysicalObject* obj1, PhysicalObject* obj2);
		uint isColliding(PhysicalObject* obj);

		bool getRayBoundingBoxCollisionPoint(PhysicalObject *obj, Line3D ray, Vector3D& point);
		bool getRayMeshCollisionPoint(PhysicalObject *obj, Line3D ray, Vector3D& point);

		void initializeNetwork(uint port);
};

#endif
