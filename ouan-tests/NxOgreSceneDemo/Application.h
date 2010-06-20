#pragma once

#include <Ogre.h>
#include <NxOgre.h>
#include <NxOgreOGRE3D.h>
#include <NxOgreAddonCharacterController.h>
#include "math.h"

#include "SimpleInputManager.h"
#include "OrbitCameraController.h"

#define NXS_PATH "file:../media/nxs"

enum GameGroup
{
   GROUP_NON_COLLIDABLE,
   GROUP_COLLIDABLE_NON_PUSHABLE,
   GROUP_COLLIDABLE_PUSHABLE,
};

#define COLLIDABLE_MASK (1 << GROUP_COLLIDABLE_NON_PUSHABLE) | (1 << GROUP_COLLIDABLE_PUSHABLE)

class Application : SimpleInputManager, NxOgre::ControllerCallback, public NxOgre::Callback
{
	public:
		Application();
		~Application();

		bool initialise();
		bool setup();

		bool createCharacter();
		bool createVolume();
		bool createBasicScenary();
		bool createTriangleMeshes();
		bool createConvexes();

		void update();
		void loadResources();

		void updateLogic(const float elapsedSeconds);
		void updateAnimations(const float elapsedSeconds);
		void updateOverlayInfo();
		
		virtual bool keyPressed( const OIS::KeyEvent& e );
		virtual bool mouseMoved( const OIS::MouseEvent& e );
	
	private:

		Ogre::Root* m_root;
		Ogre::RenderWindow* m_window;
		Ogre::SceneManager* m_sceneMgr;
		Ogre::Viewport* m_viewport;
		Ogre::Camera* m_camera;
		OrbitCameraController* m_cameraOrbitController;
		Ogre::Entity* m_character;

		Ogre::String m_debug_1;
		Ogre::String m_debug_2;

		bool onSurface;
		NxOgre::Vec3 last_hit_position;
		NxOgre::Vec3 last_hit_normal;
		NxOgre::Vec3 last_hit_sliding;
		double angle_normal;

		NxOgre::World* m_NXOgreWorld;
		NxOgre::Scene* m_NXOgreScene;
		OGRE3DRenderSystem* m_NXOgreRenderSystem;
		NxOgre::TimeController* m_NXOgreTimeController;

		NxOgre::VisualDebugger*	m_NXOgreVisualDebugger;
		OGRE3DRenderable*		m_NXOgreVisualDebuggerRenderable;
		Ogre::SceneNode*		m_NXOgreVisualDebuggerNode;

		NxOgre::ControllerManager* m_NXOgreControllerManager;
		NxOgre::Controller* m_NXOgreController;
		OGRE3DPointRenderable* m_NXOgreControllerRenderable;
		
		Ogre::AnimationState* m_idleAnimation;
		Ogre::AnimationState* m_runAnimation;
		int m_movingDirection;

		NxOgre::Volume* m_volume;

		bool m_exitRequested;
		bool m_showInfo;
		bool m_showDebug;
		
		virtual NxOgre::Enums::ControllerAction onShape(const NxOgre::ControllerShapeHit& hit);
		virtual NxOgre::Enums::ControllerAction onController(NxOgre::Controller* controller, NxOgre::Controller* other);
		void onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* rigidBody, NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent);
		void onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, void* controller, unsigned int collisionEvent);
};