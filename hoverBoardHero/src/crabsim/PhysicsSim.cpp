#include "PhysicsSim.h"

PhysicalStructure::PhysicalStructure()
{

}

PhysicalStructure::~PhysicalStructure()
{

}

PhysicalStructure::PhysicalStructure(const PhysicalStructure &other)
{
	this->bodies = other.bodies;
	this->joints = other.joints;
}

PhysicalStructure& PhysicalStructure::operator=(const PhysicalStructure &other)
{
	if (this == &other)
		return *this;

	this->bodies = other.bodies;
	this->joints = other.joints;

	return *this;
}

PhysicalObject* PhysicalStructure::getBody(stringw name)
{
	for(unsigned int i = 0; i < bodies.size(); i++)
		if(bodies[i]->getName().equals_ignore_case(name))
			return bodies[i];

	return NULL;
}

RenderTarget::RenderTarget(Camera* camera, Dimension resolution)
			: camera(camera), resolution(resolution)
{

}

RenderTarget::RenderTarget()
{

}

void PhysicsSim::setTexture(PhysicalObject* object, stringw tex_file){
    ISceneNode* isn = object->getSceneNode();
	tex_file = mediaDirectory + tex_file;
	if(smgr->getFileSystem()->existFile(tex_file))
    isn->setMaterialTexture(0, driver->getTexture(tex_file));
	isn->setMaterialFlag(EMF_LIGHTING, true);
	isn->setMaterialFlag(EMF_TEXTURE_WRAP, false);
	isn->setMaterialFlag(EMF_BACK_FACE_CULLING, true);
}

void RenderTarget::init(IrrlichtDevice* dvc)
{
	this->smgr = dvc->getSceneManager();
	this->driver = dvc->getVideoDriver();

	this->target = this->driver->addRenderTargetTexture(resolution);
}

Camera* RenderTarget::getCamera()
{
	return camera;
}

Texture* RenderTarget::getImage()
{
	return target;
}

void RenderTarget::getImageData(floatv &data)
{
	IImage* img = driver->createImage(target, position2d<s32>(0,0), target->getSize());

	uint y = img->getDimension().Height;
	uint x = img->getDimension().Width;

	for(uint i = 0; i < y; i++)
	{
		for(uint j = 0; j < x; j++)
		{
			SColor color = img->getPixel(j,i);
			data.push_back((float)(color.getRed()/255.0));
			data.push_back((float)(color.getGreen()/255.0));
			data.push_back((float)(color.getBlue()/255.0));
		}
	}

	img->drop();
	//delete img;
}

void RenderTarget::drawAll()
{
	this->driver->setRenderTarget(this->target, true, true);
	this->smgr->setActiveCamera(this->camera);
	this->draw();
	this->smgr->drawAll();
}

PhysicsSim::PhysicsSim(stringw windowTitle, uint width, uint height, bool render)
{
    broadphase = new btDbvtBroadphase();

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);

    setGravity(9.8);

	SIrrlichtCreationParameters params = SIrrlichtCreationParameters();
	params.EventReceiver = this;
	#ifndef NO_GRAPHICS
	if(render)
	{
	    params.Vsync = true;
		params.DriverType = video::EDT_OPENGL;
		params.WindowSize = core::dimension2d<u32>(width, height);
		params.Fullscreen = true;
		params.AntiAlias = 4;
		params.Stencilbuffer = false;
	}
	else
		params.DriverType = video::EDT_NULL;
	#else
	params.DriverType = video::EDT_NULL;
	#endif

	dvc = createDeviceEx(params);

	dvc->getLogger()->setLogLevel(ELL_INFORMATION);

	dvc->setWindowCaption(windowTitle.c_str());
	dvc->getCursorControl()->setVisible(false);

    driver = dvc->getVideoDriver();

	smgr = dvc->getSceneManager();

	env = dvc->getGUIEnvironment();
	pipImage = env->addImage(recti(10,10,200,200));
	pipImage->setScaleImage(true);
	pipImage->setUseAlphaChannel(false);
	pipImage->setVisible(false);

	for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
		KeyIsDown[i] = false;

	lasttick = 0;

	paused = false;

	testSphere = NULL;
	terrain = NULL;

	mediaDirectory = L"./";

	srand(time(NULL));
}

PhysicsSim::~PhysicsSim()
{
    listener = NULL;

	if(terrain != NULL)
		dynamicsWorld->removeRigidBody(terrain->getRigidBody());

    while (!joints_list.empty())
    {
        dynamicsWorld->removeConstraint(joints_list.back()->getConstraint());
        delete joints_list.back();
        joints_list.pop_back();
    }

    while (!objects_list.empty())
    {
        dynamicsWorld->removeRigidBody(objects_list.back()->getRigidBody());
        delete objects_list.back();
        objects_list.pop_back();
    }

    delete broadphase;
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;
    delete dynamicsWorld;

    dvc->drop();

}

void PhysicsSim::initializeNetwork(uint port)
{

}

Timer* PhysicsSim::getTimer()
{
    return dvc->getTimer();
}

void PhysicsSim::registerListener(ListenerFunction func)
{
	listener = static_cast<ListenerFunction>(func);
}

bool PhysicsSim::OnEvent(const SEvent& event)
{
	if(event.EventType == EET_LOG_TEXT_EVENT)
		return true;

	// Remember whether each key is down or up
	if (event.EventType == EET_KEY_INPUT_EVENT)
		KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

	if(listener != NULL)
		return listener(this, event);

	return false;

}

// This is used to check whether a key is being held down
const bool PhysicsSim::IsKeyDown(EKEY_CODE keyCode)
{
	return KeyIsDown[keyCode];
}

void PhysicsSim::setWindowTitle(stringw windowTitle)
{
	dvc->setWindowCaption(windowTitle.c_str());
}

Dimension PhysicsSim::getScreenSize()
{
	dimension2du dim = driver->getScreenSize();
	return dim;
}

void PhysicsSim::shootTestSphere()
{
	removeObject(testSphere);

	testSphere = loadSceneObject("lemon.dae");
	testSphere->setPosition(fpscam->getPosition());
	testSphere->setScale(Vector3D(2,2,2));
	PhysicalObject* phyObj = applyPhysics(testSphere, shSphere, 20);

	vector3df dir = fpscam->getTarget() - fpscam->getPosition();
	dir = 250*dir.normalize();
	phyObj->getRigidBody()->applyCentralImpulse(btVector3(dir.X, dir.Y, dir.Z));

}

void PhysicsSim::drawVector(Vector3D init, Vector3D end, Color c)
{
	Vector3D vec = end-init;
	float length = vec.getLength();

	stringw meshname("ArrowMesh");
	meshname += (int)arrows.size();
	IAnimatedMesh* arrow = smgr->addArrowMesh (meshname, c, c, 4, 8, length, (length - 0.4 < 0.0) ? length*0.4 : (length-0.4));
	IAnimatedMeshSceneNode *arrownode =	smgr->addAnimatedMeshSceneNode(arrow);

	arrownode->setPosition(init);

	//arrownode->setScale(vector3df(1.0, length, 1.0));

	vec.normalize();
	quaternion q;
	q.rotationFromTo(vector3df(0,1,0), vec);
	vector3df rot;
	q.toEuler(rot);

	arrownode->setRotation(vector3df(rot.X*180.0/M_PI, rot.Y*180.0/M_PI, rot.Z*180.0/M_PI));

	arrows.push_back(arrownode);
}

void PhysicsSim::drawLine(Line3D line)
{
	lines.push_back(line);
}

void PhysicsSim::drawLine(Vector3D p1, Vector3D p2, Color c)
{
	//driver->draw3DLine (const core::vector3df &start, const core::vector3df &end, SColor color=SColor(255, 255, 255, 255))=0
	driver->draw3DLine (p1, p2, c);
}

void PhysicsSim::drawBox(aabbox3df box, Color color)
{
	boxes.push_back(VBox(box, color));
}

void PhysicsSim::drawImage(stringw filename, Position pos)
{
	TextureInfo info;
	info.name = mediaDirectory + filename;
	info.position = pos;
	images.push_back(info);

	driver->getTexture(info.name);
}

void PhysicsSim::drawText(stringw text, Position pos, uint size, Color color)
{
	Text t;
	t.text = text;
	t.position = pos;
	t.color = color;
	t.size = size;
	texts.push_back(t);
}

Terrain* PhysicsSim::getTerrain()
{
	return (Terrain*)terrain;
}

u32 PhysicsSim::ticks()
{
	u32 tick = dvc->getTimer()->getTime();

	u32 dif = tick - lasttick;

	lasttick = tick;

	return dif;
}

void PhysicsSim::pause()
{
	paused = !paused;
}

void PhysicsSim::updatePhysics()
{
    u32 TDeltaTime = ticks();

	if(paused)
		TDeltaTime = 0;

	//if(!paused)
	//{
	dynamicsWorld->stepSimulation(TDeltaTime * 0.001f, 60);

	for(unsigned int i = 0; i < objects_list.size(); i++)
		objects_list[i]->Update();
	//}
}

void PhysicsSim::updateObjects()
{
	smgr->drawAll();
}

void PhysicsSim::setGravity(double gravity)
{
    dynamicsWorld->setGravity(btVector3(0,-gravity,0));
}

void PhysicsSim::step()
{
	btScalar dt = (btScalar)clock.getTimeMicroseconds();
	clock.reset();

	//dynamicsWorld->stepSimulation(dt, 10, 1.f/240.0f);

	dynamicsWorld->stepSimulation(dt/1000000.f);
}

bool PhysicsSim::run()
{
	return dvc->run();
}

void PhysicsSim::view(Camera* cam, RenderTarget* pipTarget)
{
	//if (dvc->isWindowActive())
	//{
	updatePhysics();
//*

	driver->beginScene(true, true, SColor(255,255,255,255));

	for(unsigned int i = 0; i < render_targets.size(); i++)
		render_targets[i]->drawAll();

	driver->setRenderTarget(0, true, true, 0);

	smgr->setActiveCamera(cam);

	smgr->drawAll();

	if(pipTarget != NULL)
	{
		pipImage->setImage(pipTarget->target);
	}

	driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
	for(uint i = 0; i < lines.size(); i++)
		drawLine(lines[i].p1, lines[i].p2, lines[i].color);
	lines.clear();

	for(uint i = 0; i < boxes.size(); i++)
		driver->draw3DBox(boxes[i].box, boxes[i].color) ;
	boxes.clear();

	for(uint i = 0; i < images.size(); i++)
	{
		ITexture* tex = driver->getTexture(images[i].name);
		Dimension dim = tex->getOriginalSize();
		driver->draw2DImage(tex, images[i].position,
                                rect<s32>(0,0,dim.Width,dim.Height),0,
                                video::SColor(255,255,255,255), true);
	}
	images.clear();

	for(uint i = 0; i < texts.size(); i++)
	{
		this->font = NULL;
		if(texts[i].size == 0)
			this->font = env->getBuiltInFont();
		else if(texts[i].size == 1)
			this->font = env->getFont(mediaDirectory + "fontcourier.bmp");
		else
			this->font = env->getFont(mediaDirectory + "bigfont.png");
		if(!this->font)
			this->font = env->getBuiltInFont();

		Dimension dim = this->font->getDimension(texts[i].text.c_str());
		this->font->draw(texts[i].text,
							rect<s32>(texts[i].position.X,texts[i].position.Y,texts[i].position.X+dim.Width,texts[i].position.Y+dim.Height),
							texts[i].color);
	}
	texts.clear();

	env->drawAll();
	driver->endScene();

	for(uint i = 0; i < arrows.size(); i++)
	{
		smgr->getMeshCache()->removeMesh(arrows[i]->getMesh());
		arrows[i]->remove();
	}
	arrows.clear();

//*/
	//}
	//else
	//	dvc->yield();
}

void PhysicsSim::setMediaDirectory(stringw dir)
{
	mediaDirectory = dir;
}

bool PhysicsSim::existFile(stringw filename)
{
	return smgr->getFileSystem()->existFile(filename);
}

Camera* PhysicsSim::addFPSCamera(Vector3D position, Vector3D target)
{

	fpscam = smgr->addCameraSceneNodeFPS(0, 25.0, 0.03, -1);
	//fpscam = smgr->addCameraSceneNodeMaya();
	fpscam->setFarValue(20000);

	//fpscam->setPosition(vector3df(0.0, 30.0 ,0.0));
	//fpscam->setTarget(vector3df(-200.0, 50.0 ,-200.0));

	//fpscam->setPosition(vector3df(0.0, 20.0, -10.0));
	//fpscam->setTarget(vector3df(0.0, 20.0, 0.0));

	//fpscam->setPosition(vector3df(-50.0, 3.0, -90.0));
	//fpscam->setTarget(vector3df(-50.0, 3.0, -80.0));

	fpscam->setPosition(position);
	fpscam->setTarget(target);

    return fpscam;
}

Camera* PhysicsSim::addCamera()
{
	return smgr->addCameraSceneNode();
}

RenderTarget* PhysicsSim::addRenderTarget(RenderTarget* renderTarget)
{
	renderTarget->init(dvc);

	render_targets.push_back(renderTarget);

	return renderTarget;
}

void PhysicsSim::removeRenderTarget(RenderTarget* rendertarget)
{
    for(unsigned int i = 0; i < render_targets.size(); i++)
    {
    	if(render_targets[i] == rendertarget)
    	{
    		rendertarget->camera->remove();
    		driver->removeTexture(rendertarget->target);

    		render_targets.erase(render_targets.begin() + i);

    		return;
    	}
    }

}

void PhysicsSim::showPip(bool show)
{
	pipImage->setVisible(show);
}

GUIEnvironment* PhysicsSim::getGUIEnvironment()
{
	return env;
}

void PhysicsSim::addTerrain(stringw heightfield_file, stringw texture_file, Vector3D scale)
{
	stringw filename = mediaDirectory + heightfield_file;
	stringw texfilename = mediaDirectory + texture_file;

	IImage* hfimg = driver->createImageFromFile (filename.c_str());

	int img_width = hfimg->getDimension().Width;
	int img_height = hfimg->getDimension().Height;

	float xScale = scale.X, yScale = scale.Y, zScale = scale.Z;

	ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(	filename.c_str(),
											smgr->getRootSceneNode(),               // parent node
											-1,                                     // node id
											vector3df(0.f, 0.f, 0.f),               // position
											vector3df(0.f, 0.f, 0.f),               // rotation
											vector3df(1.f, 1.0f, 1.f),              // scale
											video::SColor ( 255, 255, 255, 255 ),   // vertexColor
											5,                                      // maxLOD
											ETPS_17,                                // patchSize
											4                                       // smoothFactor
											);
	terrain->setMaterialTexture(0, driver->getTexture(texfilename.c_str()));
	terrain->scaleTexture(200.0f, 0);

	terrain->setScale(vector3df(xScale, yScale, zScale));
	terrain->setPosition(vector3df(-((img_width-1.0)*xScale)/2.0, 0.0, -((img_height-1.0)*zScale)/2.0));

	this->terrain = new Terrain(terrain, hfimg, Vector3D(xScale, yScale, zScale));

	dynamicsWorld->addRigidBody(this->terrain->getRigidBody());

}

void PhysicsSim::addSky(stringw skydome_file)
{
	stringw filename = mediaDirectory + skydome_file;
	smgr->addSkyDomeSceneNode(driver->getTexture(filename.c_str()), 16, 16, 1.0f, 1.9f);
}

void PhysicsSim::makeLight(vector3df position, float intensity)
{
    ILightSceneNode* lsn = smgr->addLightSceneNode(0, position, video::SColorf(1.f, 1.f, 1.f), 1);
//    smgr->setAmbientLight(SColorf(0.2, 0.2, 0.2));

    SLight sl = lsn->getLightData();
    sl.Attenuation = Vector3D(0, 0.001, 0);
    sl.DiffuseColor = SColorf(1, 1, 1);
    sl.Direction = Vector3D(0, -1, 0);
    sl.AmbientColor = SColorf(0.4, 0.4, 0.4);

    lsn->setLightData(sl);
}

SceneObject* PhysicsSim::loadSceneObject(stringw mesh_file, stringw texture_file)
{
	IAnimatedMesh *mesh = smgr->getMesh(mediaDirectory + mesh_file);

	IAnimatedMeshSceneNode* Node = smgr->addAnimatedMeshSceneNode(mesh, smgr->getRootSceneNode());
	stringw tex_file = texture_file;
	if(tex_file == L"")
	{
		tex_file = mesh_file;
		if(tex_file.find(".dae") > -1)
			tex_file.remove(".dae");
		if(tex_file.find(".3ds") > -1)
			tex_file.remove(".3ds");
		tex_file.append(".jpg");
	}
	tex_file = mediaDirectory + tex_file;
	if(smgr->getFileSystem()->existFile(tex_file))
		Node->setMaterialTexture(0, driver->getTexture(tex_file));
	Node->setMaterialFlag(EMF_LIGHTING, true);
	Node->setMaterialFlag(EMF_TEXTURE_WRAP, false);
	Node->setMaterialFlag(EMF_BACK_FACE_CULLING, true);
	Node->addShadowVolumeSceneNode(0,-1,false);
	Node->getMaterial(0).AmbientColor.set(255,255,255,255);

	updateObjects();
	return Node;
}

PhysicalObject* PhysicsSim::applyPhysics(SceneObject* object, Shape shape, float mass)
{
	PhysicalObject* phyObject;
	switch(shape)
	{
		case shSphere: phyObject = new Sphere(object, mass);
					break;

		case shBox: phyObject = new Box(object, mass);
					break;

		case shCapsule: phyObject = new Capsule(object, mass);
					break;

		case shCylinder: phyObject = new Cylinder(object, mass);
					break;

		case shCone: phyObject = new Cone(object, mass);
					break;

		case shConvexHull: phyObject = new ConvexHull(object, mass);
					break;

		case shTriMesh: phyObject = new TriMesh(object, mass);
					break;
	}

	objects_list.push_back(phyObject);
	dynamicsWorld->addRigidBody(phyObject->getRigidBody());

	updateObjects();

	return phyObject;
}

void PhysicsSim::removeObject(SceneObject* object)
{
    for(unsigned int i = 0; i < objects_list.size(); i++)
    {
    	if(objects_list[i]->getSceneNode() == object)
    	{
    		dynamicsWorld->removeRigidBody(objects_list[i]->getRigidBody());
    		delete objects_list[i];
    		objects_list.erase(objects_list.begin() + i);

    		return;
    	}
    }

    if(object != NULL) object->remove();
}

void PhysicsSim::removeObject(PhysicalObject* object)
{
    for(unsigned int i = 0; i < objects_list.size(); i++)
    {
    	if(objects_list[i] == object)
    	{
    		dynamicsWorld->removeRigidBody(objects_list[i]->getRigidBody());
    		delete objects_list[i];
    		objects_list.erase(objects_list.begin() + i);

    		return;
    	}
    }
}

Joint* PhysicsSim::createJoint(PhysicalObject* obj1, PhysicalObject* obj2, Vector3D anchor, Vector3D axis)
{
	Joint* jt = new Joint(obj1, obj2, anchor, axis);
	dynamicsWorld->addConstraint(jt->getConstraint(), true);

	joints_list.push_back(jt);

	return jt;
}

void PhysicsSim::removeJoint(Joint* joint)
{
    for(unsigned int i = 0; i < joints_list.size(); i++)
    {
    	if(joints_list[i] == joint)
    	{
    		dynamicsWorld->removeConstraint(joints_list[i]->getConstraint());
    		delete joints_list[i];
    		joints_list.erase(joints_list.begin() + i);

    		return;
    	}
    }
}

bool PhysicsSim::loadPhysicalObjects(PhysicalStructure &structure, stringc mesh_filename, Vector3D position, bool lock2d)
{

	stringc physics_filename = mesh_filename;
	physics_filename.remove(".dae");
	physics_filename.append(".bullet");
	physics_filename = mediaDirectory + physics_filename;

	if(smgr->getFileSystem()->existFile(physics_filename))
	{
		stringw collada_filename = smgr->getFileSystem()->getAbsolutePath(mediaDirectory) + mesh_filename;

		if(smgr->getFileSystem()->existFile(collada_filename))
		{
			btBlenderImporter* fileLoader = new btBlenderImporter(position);
			fileLoader->loadFile(physics_filename.c_str());

			smgr->getMesh(collada_filename);

			for(int i = 0; i < fileLoader->getNumRigidBodies(); i++)
			{
				btRigidBody* rb = (btRigidBody*)fileLoader->getRigidBodyByIndex(i);

				if(lock2d)
				{
					rb->setLinearFactor(btVector3(1,0,1));
					rb->setAngularFactor(btVector3(0,1,0));
				}

				stringc meshname(fileLoader->getNameForPointer(rb));

				stringc bodyname = meshname;
				stringc tex_file = meshname;

				s32 underline = tex_file.findFirst('_');
				if(underline < 0)
					underline = tex_file.findFirst('.');
				if(underline > -1)
					tex_file = tex_file.subString(0, underline+1);

				tex_file = mediaDirectory + tex_file + ".jpg";

				if(fileLoader->getNumRigidBodies() > 1)
					meshname = collada_filename + "#" + meshname + "-mesh";
				else
					meshname = collada_filename;
				IAnimatedMesh *mesh = smgr->getMeshCache()->getMeshByName(meshname);

				if(mesh){
					IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh, smgr->getRootSceneNode());
					node->setMaterialFlag(EMF_LIGHTING, true);
					node->setMaterialFlag(EMF_TEXTURE_WRAP, false);
					node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
					node->addShadowVolumeSceneNode(0,-1,false);

					if(smgr->getFileSystem()->existFile(tex_file))
					{
						node->setMaterialTexture(0, driver->getTexture(tex_file));
					}

					PhysicalObject* pobj = new PhysicalObject(node, rb, bodyname);
					objects_list.push_back(pobj);
					structure.bodies.push_back(pobj);
					dynamicsWorld->addRigidBody(pobj->getRigidBody());

				}

			}


			for(int i = 0; i < fileLoader->getNumConstraints(); i++)
			{
				Joint* jt = new Joint(fileLoader->getConstraintByIndex(i));
				joints_list.push_back(jt);
				structure.joints.push_back(jt);
				btTypedConstraint* constr = jt->getConstraint();
				dynamicsWorld->addConstraint(constr, true);
			}

			delete fileLoader;

			updateObjects();
			return true;
		}
	}

	return false;

}

bool PhysicsSim::loadPhysicalObjects(stringc mesh_filename, Vector3D position, bool lock2d)
{
	PhysicalStructure s;
	return loadPhysicalObjects(s, mesh_filename, position, lock2d);
}

bool PhysicsSim::isColliding(PhysicalObject* obj1, PhysicalObject* obj2)
{

	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();

		if(numContacts > 0 && ((obj1->getRigidBody() == obA && obj2->getRigidBody() == obB)
		|| (obj1->getRigidBody() == obB && obj2->getRigidBody() == obA)) ) return true;
	}

	return false;
}

uint PhysicsSim::isColliding(PhysicalObject* obj)
{
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();

	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

		uint numContacts = contactManifold->getNumContacts();

		if(numContacts > 0 && ((obj->getRigidBody() == obA || obj->getRigidBody() == obB)))
			return numContacts;

	}

	return 0;
}

bool PhysicsSim::getRayBoundingBoxCollisionPoint(PhysicalObject *obj, Line3D ray, Vector3D& point)
{
	const line3df iray(ray.p1,ray.p2);
	const ISceneNode *node;
	triangle3df tri;

	return smgr->getSceneCollisionManager()->getCollisionPoint(iray, obj->getTriangleSelector(), point, tri, node);
}

bool PhysicsSim::getRayMeshCollisionPoint(PhysicalObject *obj, Line3D ray, Vector3D& point)
{
	const line3df iray(ray.p1,ray.p2);
	const ISceneNode *node;
	triangle3df tri;

	ITriangleSelector* trisel = obj->getSceneNode()->getSceneManager()->createTriangleSelectorFromBoundingBox(obj->getSceneNode());
	bool ret = smgr->getSceneCollisionManager()->getCollisionPoint(iray, trisel, point, tri, node);
	delete trisel;
	return ret;

}
