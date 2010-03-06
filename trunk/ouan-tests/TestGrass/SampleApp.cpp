#include "ExampleApplication.h"
#include "GrassPatch.h"
#include "Blade.h"

#include <ois/OIS.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif


class MyListener : public ExampleFrameListener
{
public:
	MyListener(RenderWindow* win, Camera* cam)
		: ExampleFrameListener(win, cam)
	{
		ballPos = Vector3(0,0,0);

		size_t windowHandle = 0;
		win->getCustomAttribute( "WINDOW", &windowHandle );

		std::ostringstream windowHandleString;
		windowHandleString << windowHandle;

		OIS::ParamList parameters;
		parameters.insert( std::make_pair( std::string( "WINDOW" ), windowHandleString.str() ) );
		parameters.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_FOREGROUND" ) ) );
		parameters.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_NONEXCLUSIVE" ) ) );
		parameters.insert( std::make_pair( std::string( "w32_keyboard" ), std::string( "DISCL_FOREGROUND" ) ) );
		parameters.insert( std::make_pair( std::string( "w32_keyboard" ), std::string( "DISCL_NONEXCLUSIVE" ) ) );
		parameters.insert( std::make_pair( std::string( "x11_mouse_grab" ), std::string( "false" ) ) );
		parameters.insert( std::make_pair( std::string( "x11_mouse_hide" ), std::string( "false" ) ) );
		parameters.insert( std::make_pair( std::string( "x11_keyboard_grab" ), std::string( "false" ) ) );
		parameters.insert( std::make_pair( std::string( "XAutoRepeatOn" ), std::string( "true" ) ) );

		m_inputManager = OIS::InputManager::createInputSystem( parameters );
		m_keyboard = static_cast< OIS::Keyboard* >( m_inputManager->createInputObject( OIS::OISKeyboard, true ) );
	}
	
	Vector3 ballPos;

protected:  
    GrassPatch *mGrass;
	Entity* sphereEntity;
    SceneNode* mSphereNode;
	Entity* ground;
	SceneNode* groundNode;
	Light* light;
	SceneNode* lightNode;

	OIS::InputManager* m_inputManager;
	OIS::Keyboard* m_keyboard;

	bool frameStarted(const FrameEvent& evt)
	{
		m_keyboard->capture();

		if ( m_keyboard->isKeyDown( OIS::KC_NUMPAD4 ) ) //LEFT
		{
			ballPos.x--;
		}
		if ( m_keyboard->isKeyDown( OIS::KC_NUMPAD6 ) ) //RIGHT
		{
			ballPos.x++;
		}
		if ( m_keyboard->isKeyDown( OIS::KC_NUMPAD8 ) ) //UP
		{
			ballPos.z--;
		}
		if ( m_keyboard->isKeyDown( OIS::KC_NUMPAD2 ) ) //DOWN
		{
			ballPos.z++;
		}
		if ( m_keyboard->isKeyDown( OIS::KC_NUMPAD1 ) ) //UP
		{
			ballPos.y++;
		}
		if ( m_keyboard->isKeyDown( OIS::KC_NUMPAD3 ) )//DOWN
		{
			ballPos.y--;
		}

		///////////////////////////////////////////

	    static bool test = true;
		if (test)
        {               
            mGrass = new GrassPatch(5000, 3, 30, 30,  mCamera->getSceneManager ());
            Ogre::SceneNode* rootNode   = mCamera->getSceneManager ()->getRootSceneNode ();
            Ogre::SceneNode* mSceneNode = rootNode->createChildSceneNode (); 
			Real r = 1000;
			mGrass->createSphere("mySphereMesh", 1.5, 16, 16);
			MeshManager::getSingleton().createPlane("plane.mesh","Custom",Plane(Vector3::UNIT_Z,Vector3::ZERO),2*r,2*r,20,20);

			ground = mCamera->getSceneManager()->createEntity("Ground","plane.mesh");
			ground->setMaterialName("Examples/Rocky");
			groundNode = mCamera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
			groundNode->attachObject(ground);
			groundNode->setOrientation(Quaternion(Radian(-Math::HALF_PI),Vector3::UNIT_X));

			light = mCamera->getSceneManager()->createLight("Light1");
			light->setType(Light::LT_SPOTLIGHT);
			light->setDiffuseColour(ColourValue(0.25f,0.25f,0.0f));
			light->setSpecularColour(ColourValue(0.25f,0.25f,0.0f));
			light->setAttenuation(8000,1,0.0005,0);
			light->setSpotlightRange(Ogre::Degree(60), Ogre::Degree(70));				
			light->setDirection(Vector3::NEGATIVE_UNIT_Y);

			lightNode = mSceneNode->createChildSceneNode("LightNode");
			lightNode->attachObject(light);	

			Real y = 1.2f*r;
			lightNode->setPosition(0,y,0);

			sphereEntity = mCamera->getSceneManager()->createEntity("mySphereEntity", "mySphereMesh");
			sphereEntity->setMaterialName("Examples/TextureEffect2");
			mSphereNode = rootNode->createChildSceneNode (); 
			mSceneNode->attachObject (mGrass);
			mSphereNode->attachObject(sphereEntity); 
			mSphereNode->setPosition(0.0f,2.3f,0.0f);
			mSceneNode->showBoundingBox(true); 
			ballPos=mSphereNode->getPosition();

			mSphereNode->showBoundingBox(true); 
            mWindow->getViewport(0)->setBackgroundColour(ColourValue::Black);
            
            test = false;
        }

		mSphereNode->setPosition(ballPos);
		mGrass->Update (ballPos, 2, evt.timeSinceLastFrame); 
		//mWindow->setDebugText(mGrass->iterations);        // Call superclass
		return ExampleFrameListener::frameStarted(evt);        
	}

	bool frameEnded(const FrameEvent& evt)
	{
		return ExampleFrameListener::frameEnded(evt);        
	}

};

class MyApplication : public ExampleApplication
{
public:
    MyApplication() {}
	
protected:

    void createScene(void)
    {
        mSceneMgr->setAmbientLight(ColourValue(1.0, 1.0, 1.0));
        mCamera->setPosition (80, 80, 100);
        mCamera->lookAt(0,0,0);
     // put your scene creation in here
    }

    // Create new frame listener
    void createFrameListener(void)
    {
        mFrameListener= new MyListener(mWindow, mCamera);
        mRoot->addFrameListener(mFrameListener);      
        mFrameListener->showDebugOverlay(true);		
	}
};

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
		int main(int argc, char **argv)
#endif
		{
			// Create application object
			MyApplication app;

			try {
				app.go();
			} catch(Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
				MessageBoxA(NULL, e.getFullDescription().c_str(),
					"An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
				std::cerr << "An exception has occurred: " << e.getFullDescription();
#endif
			}

			return 0;
		}

#ifdef __cplusplus
	}
#endif