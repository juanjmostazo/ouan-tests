#ifndef __APPLICATION__H__
#define __APPLICATION__H__

#include "SimpleInputManager.h"

namespace Ogre
{
	class Root;
	class RenderWindow;
	class SceneManager;
	class Camera;
	class Viewport;

	class SceneNode;
	class Entity;
}

class OrbitCameraController;

class Application
	: SimpleInputManager
{
public:
	Application();
	virtual ~Application();

	void initialise();
	void go();

	virtual bool keyPressed( const OIS::KeyEvent& e );
	virtual bool mouseMoved( const OIS::MouseEvent& e );

private:

	void loadResources();
	void createSceneSet1();
	void createSceneSet2();
	void createSceneSet3();
	void createCamera();

	void updateLogic( const float elapsedSeconds );

	Ogre::Root* m_root;
	Ogre::RenderWindow* m_window;
	Ogre::SceneManager* m_sceneManager;
	Ogre::Camera* m_camera;
	Ogre::Viewport* m_viewport;

	OrbitCameraController* m_cameraController;

	bool m_exitRequested;
};

#endif