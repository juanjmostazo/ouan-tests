//Lighting effects test

//The code to render the flashlight halo has been taken from the 'light shafts with shadows and noise
//support' demo by Xavier Verguín González, inspired in return on the ATI paper
//http://ati.amd.com/developer/gdc/mitchell_lightshafts.pdf
//The lens flare effect adapted from the snippet on http://www.ogre3d.org/wiki/index.php/Displaying_LensFlare
//and Alan Gordie's article on http://www.gamedev.net/reference/articles/article813.asp
//Basic Ogre light examples are adapted from the tutorial on http://www.ogre3d.org/wiki/index.php/Basic_Tutorial_2

// ----------------------------------------------------------------------------
// Include the main OGRE header files
// Ogre.h just expands to including lots of individual OGRE header files
// ----------------------------------------------------------------------------
#include <Ogre.h>
#include <map>

// --------------------------------
//  Our own application framework
// ---------------------------------
#include "Application.h"

//  --------------------------------
//  Lens flare effect implementation
#include "LensFlare.h"


// ----------------------------------------------------------------------------
// Light shafts demo variables
// ----------------------------------------------------------------------------

/// Main light billboard set
Ogre::BillboardSet *gFlashlightBillboardSet= 0;
Ogre::BillboardSet *gWallLightBillboardSet = 0;
/// Camera used for rendering the shadow map and as light frustum
Ogre::Camera       *gFlashlightCamera = 0;
/// Light camera scene node
Ogre::SceneNode    *gFlashlightCameraSN = 0;
/// Camera used for rendering the shadow map and as light frustum
Ogre::Camera       *gWallLightCamera = 0;
/// Light camera scene node
Ogre::SceneNode    *gWallLightCameraSN = 0;

// Scene nodes for the entities used in the demo
Ogre::SceneNode		*gKnotSN = 0;
Ogre::SceneNode		*gFlashlightSN = 0;
Ogre::SceneNode		*gWallSN=0;

//Cookie - Flashlight colour map
typedef std::map<int,Ogre::ColourValue> TFlashlightColourMap;
TFlashlightColourMap gFlashlightColours;
void initFlashlightColourMap()
{
	gFlashlightColours.clear();
	gFlashlightColours[0]=Ogre::ColourValue::White;
	gFlashlightColours[1]=Ogre::ColourValue::White;
	gFlashlightColours[2]=Ogre::ColourValue::Red;
	gFlashlightColours[3]=Ogre::ColourValue::Green;
	gFlashlightColours[4]=Ogre::ColourValue::Blue;
}

// ----------------------
// Lens flare effect
// ----------------------
LensFlare* gLensFlare;

// Current cookie selected
int mCurrentCookie = 2;

class LightingDemoApp: public Application
{
public:
	long mKeyBuffer;
	bool mRotateEnable;
	bool mRotateKnot;

	bool mDetachedFlashlight;
	
	
	LightingDemoApp()
		:Application("Lighting demo")
		,mKeyBuffer(-1)
		,mRotateEnable(false)
		,mRotateKnot(false)
		,mDetachedFlashlight(false)
	{}
	~LightingDemoApp()
	{
	}
	void updateLogic(long elapsedTime)
	{
		if(m_camera->getPosition().y<17.5)//TODO: Replace with rayscenequery
			m_camera->setPosition(m_camera->getPosition().x,m_camera->getPosition().y+17.5,m_camera->getPosition().z);
		Application::updateLogic(elapsedTime);
		// Update light position
		updatePosition(elapsedTime);
		gLensFlare->update();
		mKeyBuffer -= elapsedTime;
	}
	bool keyPressed(const OIS::KeyEvent& evt)
	{
		Application::keyPressed(evt);
		gLensFlare->update();
		if (evt.key==OIS::KC_B && mKeyBuffer < 0)
		{
			mRotateKnot = !mRotateKnot;
			mKeyBuffer = 500;
		}
		//Turn on/off the flashlight
		if (evt.key==OIS::KC_O && mKeyBuffer<0)
		{
			if(!mDetachedFlashlight)
			{
				mDetachedFlashlight=true;
				gFlashlightCameraSN->detachObject(gFlashlightBillboardSet);
				gFlashlightCameraSN->detachObject(gFlashlightCamera);
				gFlashlightCameraSN->detachObject(m_sceneManager->getLight("Flashlight"));
			}
			else
			{
				mDetachedFlashlight=false;
				gFlashlightCameraSN->attachObject(gFlashlightBillboardSet);
				gFlashlightCameraSN->attachObject(gFlashlightCamera);
				gFlashlightCameraSN->attachObject(m_sceneManager->getLight("Flashlight"));
			}
			mKeyBuffer=500;
		}

		// Change flashlight cookie
		if (evt.key==OIS::KC_N && mKeyBuffer < 0)
		{
			mCurrentCookie++;

			if (mCurrentCookie > 4)
			{
				mCurrentCookie = 0;
			}

			Ogre::Light *mLight1 = m_sceneManager->getLight("Flashlight");
			if(mLight1)
			{
				mLight1->setDiffuseColour(gFlashlightColours[mCurrentCookie]);
				mLight1->setSpecularColour(gFlashlightColours[mCurrentCookie]);
			}

			static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("FlashlightShafts"))->
				getTechnique(0)->getPass(0)->getTextureUnitState(1)->
				setTextureName("Cookie" + Ogre::StringConverter::toString(mCurrentCookie) + ".png");

			mKeyBuffer = 500;
		}

		return true;
	}
	void updatePosition(unsigned long elapsedTime)
	{
		// Just a simple circular trajectory
		const Ogre::Real& SimulationTime = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
		Ogre::Real Radius = 3;

		//gFlashlightCameraSN->setPosition(gFlashlightCameraSN->getPosition().x,
		//	                        gFlashlightCameraSN->getPosition().y,
		//							gFlashlightCameraSN->getPosition().z);
		//gLightCameraSN->setPosition(Ogre::Math::Sin(SimulationTime/1000)*Radius,
		//	gLightCameraSN->getPosition().y,
		//	Ogre::Math::Cos(SimulationTime/1000)*Radius);

		// Set the the scene node direction to 0,KnotHeight,0 point
		/*Ogre::Vector3 NormalisedDirection = (Ogre::Vector3(gKnotSN->getPosition().x,0,0)-gLightCameraSN->getPosition()).normalisedCopy();
		gLightCameraSN->setDirection(NormalisedDirection, Ogre::Node::TS_WORLD);*/

		// Upload current position to light shafts materials
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("FlashlightShafts"))->
			getTechnique(0)->getPass(0)->getFragmentProgramParameters()->
			setNamedConstant( "uLightPosition", gFlashlightCameraSN->getPosition());
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("FlashlightShaftsDepth"))->
			getTechnique(0)->getPass(0)->getFragmentProgramParameters()->
			setNamedConstant( "uLightPosition", gFlashlightCameraSN->getPosition());

		// We've to update the texture projection matrix
        updateTextureProjectionMatrix();

		if (mRotateKnot)
		{
			gKnotSN->setOrientation(
				Ogre::Quaternion(
				      Ogre::Degree(Ogre::Root::getSingleton().getTimer()->getMilliseconds()/50),
				      Ogre::Vector3(0,1,0)));
		}
	}
	//No need to touch this ;)
	void updateTextureProjectionMatrix()
	{
		const Ogre::Matrix4 PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE(
			0.5,    0,    0,  0.5,
			0,   -0.5,    0,  0.5,
			0,      0,    1,    0,
			0,      0,    0,    1);

		Ogre::Matrix4 TexViewProj = 
			PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE * 
			gFlashlightCamera->getProjectionMatrixWithRSDepth() * 
			gFlashlightCamera->getViewMatrix(); 

		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("FlashlightShafts"))->
			getTechnique(0)->getPass(0)->getVertexProgramParameters()->
			setNamedConstant( "uTexViewProj", TexViewProj );

		TexViewProj = 
			PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE * 
			gWallLightCamera->getProjectionMatrixWithRSDepth() * 
			gWallLightCamera->getViewMatrix(); 

		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("WallLightShafts"))->
			getTechnique(0)->getPass(0)->getVertexProgramParameters()->
			setNamedConstant( "uTexViewProj", TexViewProj );

	}
	void createScene()
	{
		// Set some camera params
		m_camera->setFarClipDistance(3000);
		m_camera->setNearClipDistance(0.25f);		
		m_camera->setPosition(25,50,0);
		m_camera->lookAt(0,17.5f,0);

		//m_sceneManager->setAmbientLight(ColourValue(0.7,0.8,0.9));
		m_sceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

		m_sceneManager->setSkyBox(true,"Examples/StormySkyBox");

		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("LightShafts"))->clone("FlashlightShafts");
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("LightShafts"))->clone("WallLightShafts");
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("LightShaftsDepth"))->clone("FlashlightShaftsDepth");
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("LightShaftsDepth"))->clone("WallLightShaftsDepth");

		// Set up light 0
		initLightSphere();
		initFlashlight();
		//initWallLight();
		initRoom();

		// Set a knot
		//Ogre::Entity *mKnot = m_sceneManager->createEntity("Ogre", "ogrehead.mesh");
		Ogre::Entity *mKnot = m_sceneManager->createEntity("Knot", "knot.mesh");
		//mKnot->setMaterialName("Ogre/Skin");
		mKnot->setMaterialName("Knot");
		mKnot->setCastShadows(true);
		mKnot->setVisible(true);
		gKnotSN = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		gKnotSN->attachObject(mKnot);
		gKnotSN->setPosition(0, 17.5f, 0);
		gKnotSN->setScale(0.05, 0.05, 0.05);
	}

	void initRoom() 
	{
		// Set a floor plane
		Ogre::MovablePlane *mPlane = new MovablePlane("FloorPlane");
		mPlane->normal = Vector3::UNIT_Y;
		mPlane->d = 0;
		MeshManager::getSingleton().createPlane("FloorPlaneMesh",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *mPlane,
			1500,1500,50,50,true,1,5,5,Vector3::UNIT_Z);

		Ogre::Entity *pPlaneEnt = m_sceneManager->createEntity("Plane", "FloorPlaneMesh");
		pPlaneEnt->setMaterialName("rockwall");
		pPlaneEnt->setCastShadows(false);
		SceneNode* pPlaneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		pPlaneNode->attachObject(pPlaneEnt);
		pPlaneNode->setPosition(0,0,0);

		Ogre::Entity *mWall = m_sceneManager->createEntity("Wall","wall.mesh");
		mWall->setMaterialName("MyWallMaterial");
		mWall->setCastShadows(false);
		gWallSN = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		gWallSN->attachObject(mWall);
		gWallSN->setPosition(-200,40,0);
		gWallSN->scale(8,10,20);

		// Set up our light camera
		gWallLightCamera= m_sceneManager->createCamera("WallLightCamera");
		gWallLightCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
		// Not forget to set near+far distance in materials
		gWallLightCamera->setNearClipDistance(8);
		gWallLightCamera->setFarClipDistance(190);
		gWallLightCamera->setAspectRatio(1);
		gWallLightCamera->setFOVy(Ogre::Degree(15.0f));
		gWallLightCamera->setDebugDisplayEnabled(true);
		gWallLightCamera->setVisible(true);//Hides frustrum
		gWallLightCameraSN = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		gWallLightCameraSN->setPosition(-200,160,-50);
		gWallLightCameraSN->attachObject(gWallLightCamera);	
		gWallLightCameraSN->setDirection(Vector3(22,-17.5f,0),SceneNode::TS_WORLD);

		Ogre::Light *mLight1 = m_sceneManager->createLight("WallLight");
		//mLight1->setCastShadows(true);
		gWallLightCameraSN->attachObject(mLight1);
		//mLight1->setDebugDisplayEnabled(true);
		mLight1->setDirection(Vector3::NEGATIVE_UNIT_Z);
		mLight1->setType(Light::LT_SPOTLIGHT);
		mLight1->setDiffuseColour(Ogre::ColourValue(0.91,0.73,0.94));
		mLight1->setSpecularColour(Ogre::ColourValue(0.91,0.73,0.94));
		mLight1->setSpotlightRange(Degree(15), Degree(30));
		mLight1->setAttenuation(300,1,0.05,0.008);



		// Create our billboardset for volumetric rendering
		gWallLightBillboardSet = m_sceneManager->createBillboardSet("WallLightBillboardSet", 1);
		gWallLightBillboardSet->setMaterialName("WallLightShafts");
		gWallLightBillboardSet->setBillboardRotationType(Ogre::BBR_VERTEX);
		gWallLightBillboardSet->setCastShadows(false);
		gWallLightCameraSN->attachObject(gWallLightBillboardSet);

		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("WallLightShafts"))->
			getTechnique(0)->getPass(0)->getTextureUnitState(1)->
			setTextureName("Cookie1.png");
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("WallLightShafts"))->
			getTechnique(0)->getPass(0)->getFragmentProgramParameters()->
			setNamedConstant( "uLightFarClipDistance", gWallLightCamera->getNearClipDistance()+gWallLightCamera->getFarClipDistance());

		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("WallLightShaftsDepth"))->
			getTechnique(0)->getPass(0)->getFragmentProgramParameters()->
			setNamedConstant( "uLightFarClipDistance", gWallLightCamera->getNearClipDistance()+gWallLightCamera->getFarClipDistance());


		// Create a rush of billboards according to the frustum of the camera(gLightCamera)
		// After it, we can use the lightcamera/billboards scenenode like a light projector
		createLightShafts(gWallLightBillboardSet, gWallLightCamera, 500);

		// Creating a RRT for depth/shadow map
		createLightCameraRTT(gWallLightCamera,"WallLightShafts",gWallLightBillboardSet,"WallLightShaftsDepth", "WallLightDepthMap");


	}
	void initFlashlight() 
	{
		// Set up our light camera
		gFlashlightCamera = m_sceneManager->createCamera("FlashlightCamera");
		gFlashlightCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
		// Not forget to set near+far distance in materials
		gFlashlightCamera->setNearClipDistance(8);
		gFlashlightCamera->setFarClipDistance(100);
		gFlashlightCamera->setAspectRatio(1);
		gFlashlightCamera->setFOVy(Ogre::Degree(17.5f));
		gFlashlightCamera->setDebugDisplayEnabled(true);
		gFlashlightCamera->setVisible(false);//Hides frustrum
		gFlashlightCameraSN = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		gFlashlightCameraSN->setPosition(20,17.5f,0);
		gFlashlightCameraSN->attachObject(gFlashlightCamera);

		gFlashlightCameraSN->setDirection(-200,50, -80);

		Ogre::Entity *mFlashlight = m_sceneManager->createEntity("Flashlight","flashlight.mesh");
		mFlashlight->setMaterialName("Flashlight");
		mFlashlight->setCastShadows(true);
		gFlashlightSN = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		gFlashlightSN->attachObject(mFlashlight);
		gFlashlightSN->setPosition(19,17.5f,0);
		gFlashlightSN->yaw(Degree(-90.0));		

		Ogre::Light *mLight1 = m_sceneManager->createLight("Flashlight");
		mLight1->setType(Light::LT_SPOTLIGHT);
		mLight1->setDiffuseColour(Ogre::ColourValue::Red);
		mLight1->setSpecularColour(Ogre::ColourValue::Red);
		mLight1->setPosition(Vector3::ZERO);
		mLight1->setDirection(Vector3::NEGATIVE_UNIT_Z);
		mLight1->setSpotlightRange(Degree(15), Degree(30));
		mLight1->setAttenuation(500,1,0.03,0.004);
		gFlashlightCameraSN->attachObject(mLight1);


		// Create our billboardset for volumetric rendering
		gFlashlightBillboardSet= m_sceneManager->createBillboardSet("FlashlightBillboardSet", 1);
		gFlashlightBillboardSet->setMaterialName("FlashlightShafts");
		gFlashlightBillboardSet->setBillboardRotationType(Ogre::BBR_VERTEX);
		gFlashlightBillboardSet->setCastShadows(false);
		gFlashlightCameraSN->attachObject(gFlashlightBillboardSet);

		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("FlashlightShafts"))->
			getTechnique(0)->getPass(0)->getTextureUnitState(1)->
			setTextureName("Cookie" + Ogre::StringConverter::toString(mCurrentCookie) + ".png");

		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("FlashlightShafts"))->
			getTechnique(0)->getPass(0)->getFragmentProgramParameters()->
			setNamedConstant( "uLightFarClipDistance", gFlashlightCamera->getNearClipDistance()+gFlashlightCamera->getFarClipDistance());
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("FlashlightShaftsDepth"))->
			getTechnique(0)->getPass(0)->getFragmentProgramParameters()->
			setNamedConstant( "uLightFarClipDistance", gFlashlightCamera->getNearClipDistance()+gFlashlightCamera->getFarClipDistance());

		// Create a rush of billboards according to the frustum of the camera(gLightCamera)
		// After it, we can use the lightcamera/billboards scenenode like a light projector
		createLightShafts(gFlashlightBillboardSet, gFlashlightCamera, 150);

		// Creating a RRT for depth/shadow map
		createLightCameraRTT(gFlashlightCamera,"FlashlightShafts",gFlashlightBillboardSet,"FlashlightShaftsDepth","FlashlightDepthMap");
	}
	void initLightSphere() 
	{
		Ogre::Light *mLight0 = m_sceneManager->createLight("Light0");
		mLight0->setType(Light::LT_POINT);
		mLight0->setPosition(22000,8000,0);
		mLight0->setDiffuseColour(0.3,0.3,0);
		mLight0->setSpecularColour(0.3,0.3,0);
		mLight0->setVisible(true);
		mLight0->setAttenuation(3000,1.0,0.0019,0.000032);
		Entity* lightSphere = m_sceneManager->createEntity("LightSphere","sphere.mesh");
		//lightSphere->setMaterialName("Examples/Hilite/Yellow");
		SceneNode* lightSphereNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		lightSphereNode->attachObject(lightSphere);
		lightSphereNode->setPosition(22000,8000,0);
		lightSphereNode->scale(1,1,1);
		gLensFlare = new LensFlare(mLight0->getPosition()-m_camera->getPosition(),m_camera, m_sceneManager);
	}
	bool createLightShafts(Ogre::BillboardSet* Billboard, Ogre::Camera *LightCamera, const int &NumberOfPlanes)
	{
		// Calculate the distance between planes
		float DistanceBetweenPlanes = (LightCamera->getFarClipDistance() - LightCamera->getNearClipDistance())/NumberOfPlanes;

		// Get frustum corners to calculate near/far planes dimensions
		const Ogre::Vector3 *FrustumCorners = LightCamera->getWorldSpaceCorners();

		// Calcule near and far planes dimensions
		float NearWidth  = (FrustumCorners[0] - FrustumCorners[1]).length(),
			NearHeigth = (FrustumCorners[1] - FrustumCorners[2]).length(),
			FarWidth   = (FrustumCorners[4] - FrustumCorners[5]).length(),
			FarHeigth  = (FrustumCorners[5] - FrustumCorners[6]).length();

		// Now width/height step
		float WidthStep  = (FarWidth-NearWidth)/NumberOfPlanes,
			HeigthStep = (FarHeigth-NearHeigth)/NumberOfPlanes;

		// Add billboards
		Ogre::Billboard *CurrentBB = 0;
		for(int k = 0; k < NumberOfPlanes; k++)
		{
			CurrentBB = Billboard->createBillboard(Ogre::Vector3(0, 0,-LightCamera->getNearClipDistance() -k*DistanceBetweenPlanes), ColourValue::White);
			CurrentBB->setDimensions(NearWidth + k*WidthStep, NearHeigth + k*HeigthStep);
		}

		return true;
	}

	void createLightCameraRTT(Ogre::Camera* camera,Ogre::String materialName,Ogre::BillboardSet* billBoardSet, Ogre::String depthMaterialName,Ogre::String rttName)
	{
		// Creat a texture for use as rtt
		Ogre::TexturePtr LightCameraRTT = Ogre::TextureManager::getSingleton()
			.createManual(rttName,
			"General",
			Ogre::TEX_TYPE_2D,
			// 256*256 must be sufficient
			256, 
			256, 
			0,
			Ogre::PF_R8G8B8,
			Ogre::TU_RENDERTARGET);

		Ogre::RenderTarget* RT_Texture = LightCameraRTT->getBuffer()->getRenderTarget();

		Ogre::Viewport *RT_Texture_Viewport = RT_Texture->addViewport(camera);
		RT_Texture_Viewport->setClearEveryFrame(true);
		RT_Texture_Viewport->setBackgroundColour(Ogre::ColourValue::White);
		RT_Texture_Viewport->setOverlaysEnabled(false);
		RT_Texture_Viewport->setSkiesEnabled(false);

		// Add our depth listener
		RT_Texture->addListener(new LightDepthMapRttListener(m_sceneManager,billBoardSet,depthMaterialName));

		// Fill the texture in our material
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(materialName))->
			getTechnique(0)->getPass(0)->getTextureUnitState(0)->
			setTextureName(rttName);
	}

	// Just for change all entities materials in depth rtt
	class LightDepthMapRttListener : public Ogre::RenderTargetListener
	{
	public:
		LightDepthMapRttListener(Ogre::SceneManager *sm,Ogre::BillboardSet* billboardSet, Ogre::String depthMaterialName)
			: mSceneMgr(sm)
			, mDepthMaterialName(depthMaterialName)
			, mBillboardSet(billboardSet)
		{
		}

		void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
		{
			Ogre::SceneManager::MovableObjectIterator EntityIterator = mSceneMgr->getMovableObjectIterator("Entity");
			Ogre::Entity * CurrentEntity = NULL;
			unsigned int k = 0;

			mMaterials.empty();

			//mBillboardSet->setVisible(false);
			gFlashlightBillboardSet->setVisible(false);
			gWallLightBillboardSet->setVisible(false);

			while( EntityIterator.hasMoreElements() )
			{
				CurrentEntity = static_cast<Ogre::Entity *>(EntityIterator.peekNextValue());
				for( k = 0; k < CurrentEntity->getNumSubEntities(); k++ )
				{
					mMaterials.push(CurrentEntity->getSubEntity(k)->getMaterialName());
					CurrentEntity->getSubEntity(k)->setMaterialName(mDepthMaterialName);
				}

				EntityIterator.moveNext();
			}
		}

		void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
		{
			Ogre::SceneManager::MovableObjectIterator EntityIterator = mSceneMgr->getMovableObjectIterator("Entity");
			Ogre::Entity * CurrentEntity = NULL;
			unsigned int k = 0;

			gFlashlightBillboardSet->setVisible(true);
			gWallLightBillboardSet->setVisible(true);

			while( EntityIterator.hasMoreElements() )
			{
				CurrentEntity = static_cast<Ogre::Entity *>(EntityIterator.peekNextValue());
				for( k = 0; k < CurrentEntity->getNumSubEntities(); k++ )
				{
					CurrentEntity->getSubEntity(k)->setMaterialName(mMaterials.front());
					mMaterials.pop();
				}

				EntityIterator.moveNext();
			}
		}

		std::queue<std::string> mMaterials;	
		Ogre::SceneManager *mSceneMgr;
		Ogre::String mDepthMaterialName;
		Ogre::BillboardSet* mBillboardSet;
	};
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
		initFlashlightColourMap();
		LightingDemoApp* app=new LightingDemoApp();
		app->initialise();
		app->go();
		app->cleanUp();
		delete app;
	}
	catch(Exception& e)
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