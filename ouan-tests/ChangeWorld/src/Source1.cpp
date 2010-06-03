#include "ExampleApplication.h"
using namespace Ogre;

class RTTListener : public ExampleFrameListener
{ 
public:
   RTTListener(RenderWindow *win, Camera *cam, 
	   RenderTexture	   *renderTextureDreams,
	   RenderTexture	   *renderTextureNightmares,
	   RenderTexture	   *renderTextureNightmaresWithScreens,
	   RenderTexture	   *renderTextureChangeWorld, 
	Ogre::Rectangle2D *miniScreenDreams,
	Ogre::Rectangle2D *miniScreenNightmares,
	Ogre::Rectangle2D *miniScreenChangeWorld,
	   Entity * entity,
	   SceneNode * lightnode) 
	: ExampleFrameListener(win, cam), 
	mRenderTextureDreams(renderTextureDreams),
	mRenderTextureNightmares(renderTextureNightmares),
	mRenderTextureNightmaresWithScreens(renderTextureNightmaresWithScreens),
	mRenderTextureChangeWorld(renderTextureChangeWorld),
	mMiniScreenDreams(miniScreenDreams),
	mMiniScreenNightmares(miniScreenNightmares),
	mMiniScreenChangeWorld(miniScreenChangeWorld),
	mEntity(entity),
	mLightNode(lightnode),
	mCamera(cam)
   {
   }

   bool frameStarted(const FrameEvent& evt)
   {

	//UPDATE LIGHT
	mTime+=3*evt.timeSinceLastFrame;
	if(mTime>=Ogre::Math::TWO_PI)
	{
		mTime-=Ogre::Math::TWO_PI;
	}
	if(mTime<0)
	{
		mTime=0;
	}
	mLightNode->setPosition(mCamera->getPosition()+Vector3(0,100*Ogre::Math::Sin(mTime),0));
	

    //RENDER TO TEXTURE

	mMiniScreenDreams->setVisible(false);
	mMiniScreenNightmares->setVisible(false);
	mMiniScreenChangeWorld->setVisible(false);
	CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0),"changeworld",false);

	 unsigned int i;

	for(i=0;i<mEntity->getNumSubEntities();i++)
	{
		Ogre::SubEntity * pSubentity = mEntity->getSubEntity(i);
		pSubentity->setMaterialName("tripollo_d");
	}
		
	mRenderTextureDreams->update();
	//mRenderTextureDreams->writeContentsToFile("tripollo_d.png");

	for(i=0;i<mEntity->getNumSubEntities();i++)
	{
		Ogre::SubEntity * pSubentity = mEntity->getSubEntity(i);
		pSubentity->setMaterialName("tripollo_n");
	}
	mRenderTextureNightmares->update();
	//mRenderTextureNightmares->writeContentsToFile("tripollo_n.png");
	mMiniScreenDreams->setVisible(true);
	mMiniScreenNightmares->setVisible(true);
	mMiniScreenChangeWorld->setVisible(true);
	mRenderTextureNightmaresWithScreens->update();
	mMiniScreenDreams->setVisible(false);
	mMiniScreenNightmares->setVisible(false);
	mMiniScreenChangeWorld->setVisible(false);


	for(i=0;i<mEntity->getNumSubEntities();i++)
	{
		Ogre::SubEntity * pSubentity = mEntity->getSubEntity(i);
		pSubentity->setMaterialName("tripollo_changeworld");
	}

	mRenderTextureChangeWorld->update();
	//mRenderTextureChangeWorld->writeContentsToFile("tripollo_ChangeWorld.png");

	//PREPARE FOR OUTPUT
	mMiniScreenDreams->setVisible(true);
	mMiniScreenNightmares->setVisible(true);
	mMiniScreenChangeWorld->setVisible(true);
	for(i=0;i<mEntity->getNumSubEntities();i++)
	{
		Ogre::SubEntity * pSubentity = mEntity->getSubEntity(i);
		pSubentity->setMaterialName("tripollo_d");
	}
	CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0),"changeworld_compositor",true);

	return ExampleFrameListener::frameStarted(evt);
   }

protected:
   RenderTexture	   *mRenderTextureDreams;
   RenderTexture	   *mRenderTextureNightmares;
   RenderTexture	   *mRenderTextureNightmaresWithScreens;
   RenderTexture	   *mRenderTextureChangeWorld;

	Ogre::Rectangle2D *mMiniScreenDreams; 
	Ogre::Rectangle2D *mMiniScreenNightmares;
	Ogre::Rectangle2D *mMiniScreenChangeWorld;
   Entity	   *mEntity;
   SceneNode * mLightNode;
   Camera * mCamera;

   double mTime;
};


class TutorialApplication : public ExampleApplication
{
protected:
public:
    TutorialApplication()
    {
    }

    ~TutorialApplication() 
    {
    }
   RenderTexture	   *renderTextureDreams;
   RenderTexture	   *renderTextureNightmares;
   RenderTexture	   *renderTextureNightmaresWithScreens;
   RenderTexture	   *renderTextureChangeWorld;


	Ogre::Rectangle2D *miniScreenDreams; 
	Ogre::Rectangle2D *miniScreenNightmares;
	Ogre::Rectangle2D *miniScreenChangeWorld;

protected:
   Entity *ent1;
   Light *light;
   SceneNode * lightNode;
   SceneNode * entityNode;
    void createScene(void)
	{

		mSceneMgr->setAmbientLight( ColourValue( 0.3, 0.3, 0.3 ) );

		//CREATE MODEL
		
		ent1 = mSceneMgr->createEntity( "Model", "tripollo.mesh" );
		entityNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "EntityNode" );
		entityNode->attachObject( ent1 );
		entityNode->setScale(Vector3(15,15,15));
		entityNode->setPosition(Vector3(0,-140,0));
		entityNode->yaw(Ogre::Degree(120));

		//CREATE LIGHT

		light = mSceneMgr->createLight("Light");
		light->setDiffuseColour(1,1,1);
		lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "LightNode" );
		lightNode->attachObject(light);


		//CREATE RENDER TEXTURES
			//dreams
	   Ogre::TexturePtr textureDreams;
	   textureDreams = Ogre::TextureManager::getSingleton().createManual("renderTextureDreams",
       ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, mWindow->getWidth(), mWindow->getHeight(), 0, PF_R8G8B8,
       TU_RENDERTARGET);
	   renderTextureDreams = textureDreams->getBuffer()->getRenderTarget();
	   renderTextureDreams->addViewport(mCamera);
	   renderTextureDreams->getViewport(0)->setClearEveryFrame(true);
	   renderTextureDreams->getViewport(0)->setBackgroundColour(ColourValue::Black);
	   renderTextureDreams->setAutoUpdated(false);
	
			//nightmares
	   Ogre::TexturePtr textureNightmares;
	   textureNightmares = Ogre::TextureManager::getSingleton().createManual("renderTextureNightmares",
       ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, mWindow->getWidth(), mWindow->getHeight(), 0, PF_R8G8B8,
       TU_RENDERTARGET);
	   renderTextureNightmares = textureNightmares->getBuffer()->getRenderTarget();
	   renderTextureNightmares->addViewport(mCamera);
	   renderTextureNightmares->getViewport(0)->setClearEveryFrame(true);
	   renderTextureNightmares->getViewport(0)->setBackgroundColour(ColourValue::Black);
	   renderTextureNightmares->setAutoUpdated(false);

			//nightmares with screens
	   Ogre::TexturePtr textureNightmaresWithScreens;
	   textureNightmaresWithScreens = Ogre::TextureManager::getSingleton().createManual("renderTextureNightmaresWithScreens",
       ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, mWindow->getWidth(), mWindow->getHeight(), 0, PF_R8G8B8,
       TU_RENDERTARGET);
	   renderTextureNightmaresWithScreens = textureNightmaresWithScreens->getBuffer()->getRenderTarget();
	   renderTextureNightmaresWithScreens->addViewport(mCamera);
	   renderTextureNightmaresWithScreens->getViewport(0)->setClearEveryFrame(true);
	   renderTextureNightmaresWithScreens->getViewport(0)->setBackgroundColour(ColourValue::Black);
	   renderTextureNightmaresWithScreens->setAutoUpdated(false);

			//ChangeWorld
	   Ogre::TexturePtr textureChangeWorld;
	   textureChangeWorld = Ogre::TextureManager::getSingleton().createManual("renderTextureChangeWorld",
       ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, mWindow->getWidth(), mWindow->getHeight(), 0, PF_R8G8B8,
       TU_RENDERTARGET);
	   renderTextureChangeWorld = textureChangeWorld->getBuffer()->getRenderTarget();
	   renderTextureChangeWorld->addViewport(mCamera);
	   renderTextureChangeWorld->getViewport(0)->setClearEveryFrame(true);
	   renderTextureChangeWorld->getViewport(0)->setBackgroundColour(ColourValue::Black);
	   renderTextureChangeWorld->setAutoUpdated(false);

		//CREATE MINISCREENS
		Ogre::Technique * technique;
		//DREAMS
			//miniscreen
		miniScreenDreams = new Ogre::Rectangle2D(true);
		miniScreenDreams->setCorners(0.5, 1.0, 1.0, 0.5);
		miniScreenDreams->setBoundingBox(AxisAlignedBox(-100000.0*Vector3::UNIT_SCALE, 100000.0*Vector3::UNIT_SCALE));
		Ogre::SceneNode *miniScreenDreamsNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("miniScreenDreamsNode");
		miniScreenDreamsNode->attachObject(miniScreenDreams);
			//material
		MaterialPtr materialDreams = MaterialManager::getSingleton().create("miniScreenMatDreams", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		technique = materialDreams->createTechnique();
		technique->createPass();
		materialDreams->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		materialDreams->getTechnique(0)->getPass(0)->createTextureUnitState("renderTextureDreams");
		miniScreenDreams->setMaterial("miniScreenMatDreams");

		//NIGHTMARES
		miniScreenNightmares = new Ogre::Rectangle2D(true);
		miniScreenNightmares->setCorners(0, 1.0, 0.5, 0.5);
		miniScreenNightmares->setBoundingBox(AxisAlignedBox(-100000.0*Vector3::UNIT_SCALE, 100000.0*Vector3::UNIT_SCALE));
		Ogre::SceneNode *miniScreenNightmaresNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("miniScreenNightmaresNode");
		miniScreenNightmaresNode->attachObject(miniScreenNightmares);
			//material
		MaterialPtr materialNightmares = MaterialManager::getSingleton().create("miniScreenMatNightmares", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		technique = materialNightmares->createTechnique();
		technique->createPass();
		materialNightmares->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		materialNightmares->getTechnique(0)->getPass(0)->createTextureUnitState("renderTextureNightmares");
		miniScreenNightmares->setMaterial("miniScreenMatNightmares");

		//CHANGEWORLD
		miniScreenChangeWorld = new Ogre::Rectangle2D(true);
		miniScreenChangeWorld->setCorners(-0.5, 1.0, 0, 0.5);
		miniScreenChangeWorld->setBoundingBox(AxisAlignedBox(-100000.0*Vector3::UNIT_SCALE, 100000.0*Vector3::UNIT_SCALE));
		Ogre::SceneNode *miniScreenChangeWorldNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("miniScreenChangeWorldNode");
		miniScreenChangeWorldNode->attachObject(miniScreenChangeWorld);
			//material
		MaterialPtr materialChangeWorld = MaterialManager::getSingleton().create("miniScreenMatChangeWorld", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		technique = materialChangeWorld->createTechnique();
		technique->createPass();
		materialChangeWorld->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		materialChangeWorld->getTechnique(0)->getPass(0)->createTextureUnitState("renderTextureChangeWorld");
		miniScreenChangeWorld->setMaterial("miniScreenMatChangeWorld");

		//FINAL SCREEN
		   //set textures to changeworld compositor
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("changeworld");
		technique= material->getTechnique(0);
		technique->getPass(0)->getTextureUnitState(0)->setTextureName("renderTextureChangeWorld");
		technique->getPass(0)->getTextureUnitState(2)->setTextureName("renderTextureNightmaresWithScreens");
		  //add compositor
	    CompositorManager::getSingleton().addCompositor(mWindow->getViewport(0),"changeworld_compositor");

    }
   // Create a new frame listener
   void createFrameListener()
   {
       mFrameListener = new RTTListener(mWindow, mCamera,
		   renderTextureDreams,renderTextureNightmares,renderTextureNightmaresWithScreens,renderTextureChangeWorld,
		   miniScreenDreams,miniScreenNightmares,miniScreenChangeWorld,
		   ent1,lightNode);
       mRoot->addFrameListener(mFrameListener);

		Overlay* overlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");   
		overlay->hide();
   }

};




#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    TutorialApplication app;

    try {
        app.go();
    } catch( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
        MessageBoxA( NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n",
                e.what());
#endif
    }

    return 0;
}
