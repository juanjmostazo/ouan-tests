// ----------------------------------------------------------------------------
// Include the main OGRE header files
// Ogre.h just expands to including lots of individual OGRE header files
// ----------------------------------------------------------------------------
#include <Ogre.h>

//
#include "Hydrax/Hydrax.h"
#include "Hydrax/Noise/Perlin/Perlin.h"
#include "Hydrax/Modules/ProjectedGrid/ProjectedGrid.h"

//
//#include "Caelum.h"

#define SKYBOX "Sky/EarlyMorning"
#define SUN_POSITION Ogre::Vector3(0,10000,90000)
#define SUN_COLOUR Ogre::ColourValue(1,0.6,0.4)

// --------------------------------
//  Our own application framework
// ---------------------------------
#include "Application.h"

class HydraXDemoApp: public Application
{
private:
	// Hydrax object
	Hydrax::Hydrax *mHydrax;
	long mKeyBuffer;
protected:
	void setupScene()
	{
		m_sceneManager = m_root->createSceneManager("TerrainSceneManager");
		createCameras();
		createViewports();	
		createScene();
	}
public:
	HydraXDemoApp()
		:Application("HydraX demo app")
		,mKeyBuffer(-1)
	{
		mHydrax=NULL;
	}
	
	void updateLogic(float elapsedTime)
	{
		checkCameraHeight();
		Application::updateLogic(elapsedTime);
		mHydrax->update(elapsedTime);
		//mKeyBuffer-=elapsedTime;
	}
	bool keyPressed(const OIS::KeyEvent& evt)
	{
		Application::keyPressed(evt);
		return true;
	}
	void checkCameraHeight()
	{
		Ogre::RaySceneQuery *raySceneQuery = 
			m_sceneManager->
			createRayQuery(Ogre::Ray(m_camera->getPosition() + Ogre::Vector3(0,10000,0), 
			Ogre::Vector3::NEGATIVE_UNIT_Y));
		Ogre::RaySceneQueryResult& qryResult = raySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator i = qryResult.begin();
		if (i != qryResult.end() && i->worldFragment)
		{
			if (m_camera->getPosition().y < i->worldFragment->singleIntersection.y + 30)
			{
				m_camera->setPosition(m_camera->getPosition().x, i->worldFragment->singleIntersection.y + 30, m_camera->getPosition().z);
			}
		}

		delete raySceneQuery;
	}
	void createScene()
	{
		m_sceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));
		m_sceneManager->setSkyBox(true,SKYBOX,99999*3,true);
		m_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

		m_camera->setNearClipDistance(1.5f);
		m_camera->setFarClipDistance(99999*6);
		m_camera->setPosition(312.902,206.419,1524.02);
		m_camera->setOrientation(Ogre::Quaternion(0.998,-0.0121,-0.0608,-0.00074));

		Ogre::Light* light = m_sceneManager->createLight("Sun");
		light->setPosition(SUN_POSITION);
		light->setDiffuseColour(Ogre::ColourValue::White);
		light->setSpecularColour(SUN_COLOUR);

		////Hydrax initialization code
		mHydrax = new Hydrax::Hydrax(m_sceneManager,m_camera,m_window->getViewport(0));
		////Create projected grid module
		Hydrax::Module::ProjectedGrid * module = 
			new Hydrax::Module::ProjectedGrid(
											mHydrax,
											new Hydrax::Noise::Perlin(),
											Ogre::Plane(Ogre::Vector3(0,1,0),Ogre::Vector3(0,0,0)),
											Hydrax::MaterialManager::NM_VERTEX,
											Hydrax::Module::ProjectedGrid::Options());
		mHydrax->setModule(static_cast<Hydrax::Module::Module*>(module));
		mHydrax->loadCfg("HydraxDemo.hdx");
		mHydrax->create();

		m_sceneManager->setWorldGeometry("Island.cfg");

		mHydrax->getMaterialManager()->addDepthTechnique(
			static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"))
			->createTechnique());

	}

};
	
// ----------------------------------------------------------------------------
// Main function, just boots the application object
// ----------------------------------------------------------------------------
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
	try
	{
		HydraXDemoApp* app=new HydraXDemoApp();
		app->initialise();
		app->go();
		app->cleanUp();
		delete app;
	}
	catch(Ogre::Exception& e)
	{
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occurred: %s\n",
			e.getFullDescription().c_str());
#endif
	}

	return 0;
}