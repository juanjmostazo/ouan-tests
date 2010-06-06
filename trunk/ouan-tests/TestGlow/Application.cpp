#include "Application.h"

#include "OrbitCameraController.h"
#include <Ogre.h>

Application::Application()
: m_root( NULL )
, m_window( NULL )
, m_sceneManager( NULL )
, m_camera( NULL )
, m_viewport( NULL )
, m_sceneNode( NULL )
, m_entity( NULL )
, m_cameraController( NULL )
, m_exitRequested( false )
{

}

Application::~Application()
{
	delete m_cameraController;

	SimpleInputManager::finalise();
	delete m_root;
}

void Application::initialise()
{

	//m_root = new Ogre::Root( "ogre_plugins.txt", "ogre_configuration.txt", "ogre_log.txt" );
	m_root = new Ogre::Root();

	// Initialise display options.
	// It shows the ugly dialog at start-up, so if you don't want to see it, it's up to you 
	// to remove this line and initialise display options manually ( e.g. reading them from 
	// a text file ).
	bool configDialogUserContinue = m_root->showConfigDialog();
	if ( ! configDialogUserContinue )
	{
		throw std::exception( "User closed/canceled config dialog" );
	}

	// Create window with chosen display options.
	m_window = m_root->initialise( true, "Ogre Window" );

	m_sceneManager = m_root->createSceneManager( Ogre::ST_GENERIC );

	m_camera = m_sceneManager->createCamera( "DefaultCamera" );

	m_viewport = m_window->addViewport( m_camera );

	m_viewport->setBackgroundColour( Ogre::ColourValue( 0.5, 0.5, 1 ) );

	SimpleInputManager::initialise( m_window );

	loadResources();

	createBasicScene();
	createScene();
}

void Application::loadResources()
{
	// You can specify as many locations as you want, or have several resource groups that you
	// load/unload when you need.
	// You may also wish to read the resource locations from a configuration file instead of
	// having them hard-coded.
	Ogre::ResourceGroupManager& resourceManager = Ogre::ResourceGroupManager::getSingleton();

	Ogre::String secName, typeName, archName;
	Ogre::ConfigFile cf;
	cf.load("resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	resourceManager.initialiseAllResourceGroups();
}

void Application::go()
{
	Ogre::Timer loopTimer;

	bool continueRunning = true;
	while ( continueRunning )
	{
		Ogre::WindowEventUtilities::messagePump();
		
		SimpleInputManager::capture();

		// Update logic stuff
		float elapsedSeconds = loopTimer.getMicroseconds() * 1.0 / 1000000;
		updateLogic( elapsedSeconds );

		// Update graphics stuff
		
		bool windowClosed = m_window->isClosed();
		continueRunning &= ! windowClosed;

		loopTimer.reset();
		bool renderFrameSuccess = m_root->renderOneFrame();
		continueRunning &= renderFrameSuccess;

		continueRunning &= ! m_exitRequested;		
	}
}


void Application::updateLogic( const float elapsedSeconds )
{
	const float TURN_DEGREES_PER_SECOND = 360;
	const float MOVEMENT_UNITS_PER_SECOND = 4;

	if ( m_keyboard->isKeyDown( OIS::KC_LEFT ) )
	{
		m_sceneNode->yaw( Ogre::Degree( TURN_DEGREES_PER_SECOND * elapsedSeconds ) );
	}
	if ( m_keyboard->isKeyDown( OIS::KC_RIGHT ) )
	{
		m_sceneNode->yaw( Ogre::Degree( -TURN_DEGREES_PER_SECOND * elapsedSeconds ) );
	}
	if ( m_keyboard->isKeyDown( OIS::KC_UP ) )
	{
		m_sceneNode->translate( Ogre::Vector3::UNIT_Z * MOVEMENT_UNITS_PER_SECOND * elapsedSeconds, Ogre::Node::TS_LOCAL );
	}
	if ( m_keyboard->isKeyDown( OIS::KC_DOWN ) )
	{
		m_sceneNode->translate( Ogre::Vector3::NEGATIVE_UNIT_Z * MOVEMENT_UNITS_PER_SECOND * elapsedSeconds, Ogre::Node::TS_LOCAL );
	}
}


bool Application::keyPressed( const OIS::KeyEvent& e )
{
	if ( e.key == OIS::KC_ESCAPE )
	{
		m_exitRequested = true;
	}

	if ( e.key == OIS::KC_1 )
	{
		glowNodeFlag = !glowNodeFlag;		
	}

	if ( e.key == OIS::KC_2 )
	{
		alphaGlowNodeFlag = !alphaGlowNodeFlag;
	}

	if ( e.key == OIS::KC_3 )
	{
		noDepthGlowNodeFlag = !noDepthGlowNodeFlag;
	}

	glowNode->setVisible(glowNodeFlag);
	alphaGlowNode->setVisible(alphaGlowNodeFlag);
	noDepthGlowNode->setVisible(noDepthGlowNodeFlag);

	return true;
}

bool Application::mouseMoved( const OIS::MouseEvent& e )
{
	assert( m_cameraController != NULL );

	bool leftMouseButtonPressed = e.state.buttonDown( OIS::MB_Left );
	if ( leftMouseButtonPressed )
	{
		m_cameraController->addOrientation( -e.state.X.rel, -e.state.Y.rel );
	}
	m_cameraController->addDistance( -e.state.Z.rel * 0.01 );

	return true;
}

void Application::createBasicScene()
{
	m_sceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();

	Ogre::Entity* groundEntity = m_sceneManager->createEntity( "Ground", "ground.mesh" );
	Ogre::SceneNode* groundSceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	groundSceneNode->attachObject( groundEntity );
	groundSceneNode->setScale( 500, 1, 500 );

	m_cameraController = new OrbitCameraController( m_camera );
	m_cameraController->setOrientation( -30, -15 );
	m_cameraController->setDistance( 250 );
	m_cameraController->setLookAtPosition( 0, 1, 0 );
	m_camera->setNearClipDistance( 0.01 );
}

void Application::createScene()
{
	// normal entity
	Ogre::Entity *normalOgreEntity = m_sceneManager->createEntity("normalOgreEntity", "ogrehead.mesh");
	Ogre::SceneNode* normalOgreNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	normalOgreNode->attachObject(normalOgreEntity);
	normalOgreNode->setPosition(80, 20, 0);

	// glow entity
	Ogre::Entity *fullGlowEntity = m_sceneManager->createEntity("fullGlowEntity", "ogrehead.mesh");
	Ogre::SceneNode* testNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	testNode->attachObject(fullGlowEntity);
	testNode->setPosition(-80, 20, 0);

	////////////////////////////////////////////

	glowNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	alphaGlowNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	noDepthGlowNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();

	glowNode->setPosition(-80, 20, 0);
	alphaGlowNode->setPosition(-80, 20, 0);
	noDepthGlowNode->setPosition(-80, 20, 0);

	////////////////////////////////////////////

	// glow
	Ogre::Entity* glowEntity = fullGlowEntity->clone(fullGlowEntity->getName() + "_glow"); 
	glowEntity->setMaterialName("cg/glow"); 
	glowNode->attachObject(glowEntity); 
	glowNodeFlag = false;

	// alpha glow
	Ogre::Entity* alphaGlowEntity = fullGlowEntity->clone(fullGlowEntity->getName() + "_alpha_glow"); 
	alphaGlowEntity->setMaterialName("cg/alpha_glow"); 
	alphaGlowNode->attachObject(alphaGlowEntity); 
	alphaGlowNodeFlag = false;

	// check glow
	Ogre::Entity* noDepthCheckGlowEntity = fullGlowEntity->clone(fullGlowEntity->getName() + "_no_depth_check_glow"); 
	noDepthCheckGlowEntity->setMaterialName("cg/no_depth_check_glow"); 
	noDepthGlowNode->attachObject(noDepthCheckGlowEntity); 
	noDepthGlowNodeFlag = false;

	////////////////////////////////////////////

	glowNode->setVisible(glowNodeFlag);
	alphaGlowNode->setVisible(alphaGlowNodeFlag);
	noDepthGlowNode->setVisible(noDepthGlowNodeFlag);
}