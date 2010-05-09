#ifndef __APPLICATION__H__
#define __APPLICATION__H__

#include <Ogre.h>
#include "SimpleInputManager.h"
#include "OrbitCameraController.h"

#include <OgreTexture.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreTextureManager.h>
#include <OgreLogManager.h>

#include "VolumeRenderable.h"
#include "ThingRenderable.h"

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

	void createScene();

	void createLight();
	void createGround();
	void createOgreHead();

	Ogre::SimpleRenderable* createVolumeEffect(
		Ogre::TexturePtr ptex, 
		Ogre::Vector3 globalPos,
		Ogre::Vector2 volumeParams,
		Ogre::Vector3 quadParams, 
		Ogre::Vector3 juliaParams, 
		Ogre::Vector2 otherParams);

	void createCamera();

	void updateLogic( const float elapsedSeconds );

	Ogre::Root* m_root;
	Ogre::RenderWindow* m_window;
	Ogre::SceneManager* m_sceneManager;
	Ogre::Camera* m_camera;
	Ogre::Viewport* m_viewport;

	Ogre::AnimationState* mOgreHeadAnimState;
	OrbitCameraController* m_cameraController;

	bool m_exitRequested;
	bool m_headAnim;

	int m_numQuadRends;
	Ogre::SimpleRenderable** m_quadRends;
	float m_quadSpeedScale;
};

#endif
