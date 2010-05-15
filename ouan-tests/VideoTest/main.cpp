
// ----------------------------------------------------------------------------
// Include the main OGRE header files
// Ogre.h just expands to including lots of individual OGRE header files
// ----------------------------------------------------------------------------
#include <Ogre.h>
#include <map>

#include "OgreDirectShow.h"

// --------------------------------
//  Our own application framework
// ---------------------------------
#include "Application.h"

#define MOVIE_PATH "C:/Users/Tania/Downloads/few.dollars/For.A.Few.Dollars.More.1965.528.634Kbps.29.97fps.96kbsmp3.avi"

class VideoDemoApp: public Application
{
public:
	long mKeyBuffer;
	OgreUtils::DirectShowMovieTexture* dshowMovieTextureSystem;
	Ogre::Rectangle2D* mScreen;
	
	
	VideoDemoApp()
		:Application("Video demo")
		,mKeyBuffer(-1)
		,dshowMovieTextureSystem(NULL)
		,mScreen(NULL)
	{}
	~VideoDemoApp()
	{
	}
	void updateLogic(long elapsedTime)
	{
		Application::updateLogic(elapsedTime);
		dshowMovieTextureSystem->updateMovieTexture();	
		mKeyBuffer -= elapsedTime;
	}
	bool keyPressed(const OIS::KeyEvent& evt)
	{
		Application::keyPressed(evt);

		if (evt.key==OIS::KC_P && mKeyBuffer < 0)
		{
			if (!dshowMovieTextureSystem->isPlayingMovie())
				dshowMovieTextureSystem->playMovie();
			else
				dshowMovieTextureSystem->pauseMovie();
			mKeyBuffer = 500;
		}
		if (evt.key==OIS::KC_S && mKeyBuffer<0)
		{
			if (dshowMovieTextureSystem->isPlayingMovie())
			{
				dshowMovieTextureSystem->stopMovie();
				dshowMovieTextureSystem->rewindMovie();
			}
			mKeyBuffer=500;
		}

		// Change flashlight cookie
		if (evt.key==OIS::KC_R && mKeyBuffer < 0)
		{
			dshowMovieTextureSystem->rewindMovie();
			mKeyBuffer = 500;
		}

		return true;
	}
	void createScene()
	{
		//m_sceneManager->setAmbientLight(ColourValue(0.7,0.8,0.9));
		dshowMovieTextureSystem=new OgreUtils::DirectShowMovieTexture(640, 480);
		try
		{
			dshowMovieTextureSystem->loadMovie(MOVIE_PATH);
			Ogre::Vector2 movDim = dshowMovieTextureSystem->getMovieDimensions();
			//This hack is used to retrieve the actual movie dimensions. Apparently, it prevents
			//the app from crashing when loading large files.
			delete dshowMovieTextureSystem;
			dshowMovieTextureSystem = new OgreUtils::DirectShowMovieTexture(movDim.x, movDim.y);

			dshowMovieTextureSystem->loadMovie(MOVIE_PATH);

		}
		catch (std::string s)
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("ERROR loading movie!!");
			dshowMovieTextureSystem=NULL;
		}
		
		if (dshowMovieTextureSystem)
		{
			Ogre::Vector2 movDim = dshowMovieTextureSystem->getMovieDimensions();
			delete dshowMovieTextureSystem;
			dshowMovieTextureSystem = new OgreUtils::DirectShowMovieTexture(movDim.x, movDim.y);

			dshowMovieTextureSystem->loadMovie(MOVIE_PATH);
			mScreen= new Ogre::Rectangle2D(true);
			mScreen->setCorners(-1.0, 1.0, 1.0, -1.0);	
			mScreen->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
			mScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0*Ogre::Vector3::UNIT_SCALE, 100000.0*Ogre::Vector3::UNIT_SCALE));

			// Create background material
			Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("Movie", "General");
			material->getTechnique(0)->getPass(0)->createTextureUnitState(dshowMovieTextureSystem->getMovieTexture()->getName());
			material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
			material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
			material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
			mScreen->setMaterial("Movie");


			Ogre::SceneNode* screenNode = m_sceneManager->getRootSceneNode()->createChildSceneNode("SCREEN");
			screenNode ->attachObject(mScreen);
			dshowMovieTextureSystem->playMovie();
		}
		else
			m_exitRequested=true;
	}
	void cleanUp()
	{
		Ogre::String materialName="Movie";
		if (Ogre::MaterialManager::getSingleton().resourceExists(materialName))
		{
			Ogre::MaterialPtr mat;
			Ogre::TextureUnitState* tex;

			mat=Ogre::MaterialManager::getSingleton().getByName("Movie");
			tex=mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
			tex->setTextureName(Ogre::String(""));
		}
		if (dshowMovieTextureSystem)
		{
			delete dshowMovieTextureSystem;
			dshowMovieTextureSystem=NULL;	
		}
		if (mScreen)
		{
			delete mScreen;
			mScreen=NULL;
		}
		Application::cleanUp();
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
		VideoDemoApp* app=new VideoDemoApp();
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