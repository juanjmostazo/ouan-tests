#include "Application.h"

#include "Julia.h"

Application::Application()
: m_root( NULL )
, m_window( NULL )
, m_sceneManager( NULL )
, m_camera( NULL )
, m_viewport( NULL )
, m_cameraController( NULL )
, m_exitRequested( false )
, m_headAnim( false )
, m_numQuadRends ( 1 )
, m_quadSpeedScale ( 0.1f )
, m_unique_id ( 0 )
{

}

Application::~Application()
{
	delete m_cameraController;
	SimpleInputManager::finalise();

	//ptex.setNull();
	//delete vrend;
	//delete trend;

	delete m_root;
}

void Application::initialise()
{
	//m_root = new Ogre::Root( "ogre_plugins.txt", "ogre_configuration.txt", "ogre_log.txt" );
	m_root = new Ogre::Root();

	Ogre::ConfigFile cf;
    cf.load("resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	std::string secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

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

	m_viewport->setBackgroundColour( Ogre::ColourValue( 0, 0, 0 ) );

	SimpleInputManager::initialise( m_window );

	loadResources();

	createScene();

	createCamera();
}

void Application::loadResources()
{
	// You can specify as many locations as you want, or have several resource groups that you
	// load/unload when you need.
	// You may also wish to read the resource locations from a configuration file instead of
	// having them hard-coded.

	Ogre::ResourceGroupManager& resourceManager = Ogre::ResourceGroupManager::getSingleton();

	//resourceManager.addResourceLocation( "../data", "FileSystem" );
	//resourceManager.addResourceLocation( "../media", "FileSystem" );

	resourceManager.initialiseAllResourceGroups();
}

void Application::createCamera()
{
	m_cameraController = new OrbitCameraController( m_camera );
	m_cameraController->setOrientation( -45, -25 );
	m_cameraController->setDistance( 500 );
	m_cameraController->setLookAtPosition( 0, 10, 0 );
	m_camera->setNearClipDistance( 0.01 );
}

void Application::createScene()
{
	//createLight();
	
	if (m_headAnim) 
	{
		createOgreHead();
	}

	//////////////////////////////////////////////////////////////////////////////////////

	Ogre::TexturePtr ptex = Ogre::TextureManager::getSingleton().createManual(
			"DynaTexTest","General", Ogre::TEX_TYPE_3D, 64, 64, 64, 0, Ogre::PF_A8R8G8B8);

	int indexQuadRends = 0;
	m_numQuadRends = 4;
	m_quadRends = (Ogre::SimpleRenderable**) malloc(sizeof(Ogre::SimpleRenderable*) * m_numQuadRends);
	
	float gap = 75;
	float height = 100;

	float volume_slices = 32;
	float volume_size = 750.f;

	float quads_radius = 90.0f;
	float quads_number = 128;
	float quads_size = 1.5f;

	float global_real = 0.5f;
	float global_imag = 0.5f;
	float global_theta = 0.2f;

	float scale = 2.5f;
	float vcut = 29.0f;

	Ogre::SceneNode* parent = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	parent->setPosition(0, 0, 0);

	m_quadRends[indexQuadRends++] = createVolumeEffect(
		parent,
		ptex,
		Ogre::Vector3(0, 0, -gap),
		Ogre::Vector2(volume_slices, volume_size),
		Ogre::Vector3(quads_radius, quads_number, quads_size),
		Ogre::Vector3(global_real, global_imag, global_theta),
		Ogre::Vector2(scale, vcut));

	m_quadRends[indexQuadRends++] = createVolumeEffect(
		parent,
		ptex,
		Ogre::Vector3(gap, 0, 0),
		Ogre::Vector2(volume_slices, volume_size),
		Ogre::Vector3(quads_radius, quads_number, quads_size),
		Ogre::Vector3(global_real, global_imag, global_theta),
		Ogre::Vector2(scale, vcut));

	m_quadRends[indexQuadRends++] = createVolumeEffect(
		parent,
		ptex,
		Ogre::Vector3(0, 0, gap),
		Ogre::Vector2(volume_slices, volume_size),
		Ogre::Vector3(quads_radius, quads_number, quads_size),
		Ogre::Vector3(global_real, global_imag, global_theta),
		Ogre::Vector2(scale, vcut));

	m_quadRends[indexQuadRends++] = createVolumeEffect(
		parent,
		ptex,
		Ogre::Vector3(-gap, 0, 0),
		Ogre::Vector2(volume_slices, volume_size),
		Ogre::Vector3(quads_radius, quads_number, quads_size),
		Ogre::Vector3(global_real, global_imag, global_theta),
		Ogre::Vector2(scale, vcut));
}

Ogre::SimpleRenderable* Application::createVolumeEffect(Ogre::SceneNode* parent, Ogre::TexturePtr ptex, Ogre::Vector3 pos, Ogre::Vector2 volumeParams, Ogre::Vector3 quadParams, Ogre::Vector3 juliaParams, Ogre::Vector2 otherParams)
{
	float global_real = juliaParams.x;
	float global_imag = juliaParams.y;
	float global_theta = juliaParams.z;

	const float scale = otherParams.x;
	const float vcut = otherParams.y;
	const float vscale = 1.0f/vcut;

	float volume_slices = volumeParams.x;
	float volume_size = volumeParams.y;

	float quads_radius = quadParams.x;
	float quads_number = quadParams.y;
	float quads_size = quadParams.z;

	Ogre::SceneNode* snode = parent->createChildSceneNode();
	snode->setPosition(pos);
	
	Ogre::SimpleRenderable* vrend = new VolumeRenderable(volume_slices, volume_size, ptex->getName());
    snode->attachObject( vrend );
	
	Ogre::SimpleRenderable* trend = new ThingRenderable(quads_radius, quads_number, quads_size);
	trend->setMaterial("Examples/VTDarkStuff");
	snode->attachObject(trend);

	Ogre::Entity* cloudGroundEntity = m_sceneManager->createEntity( "Ground_" + Ogre::StringConverter::toString(Ogre::Real(m_unique_id++)), "ground.mesh" );
	Ogre::SceneNode* groundSceneNode = snode->createChildSceneNode();
	groundSceneNode->attachObject( cloudGroundEntity );
	groundSceneNode->setScale(volume_size/4, 1, volume_size/4);

	Julia julia(global_real, global_imag, global_theta);

	Ogre::HardwarePixelBufferSharedPtr buffer = ptex->getBuffer(0, 0);
	std::stringstream d;
	d << "HardwarePixelBuffer " << buffer->getWidth() << " " << buffer->getHeight() << " " << buffer->getDepth();
	Ogre::LogManager::getSingleton().logMessage(d.str());
	
	buffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pb = buffer->getCurrentLock();
	d.str("");
	d << "PixelBox " << pb.getWidth() << " " << pb.getHeight() << " " << pb.getDepth() << " " << pb.rowPitch << " " << pb.slicePitch << " " << pb.data << " " << Ogre::PixelUtil::getFormatName(pb.format);
	Ogre::LogManager::getSingleton().logMessage(d.str());
	
	unsigned int* pbptr = static_cast<unsigned int*>(pb.data);
	for(size_t z=pb.front; z<pb.back; z++) 
    {
        for(size_t y=pb.top; y<pb.bottom; y++)
        {
            for(size_t x=pb.left; x<pb.right; x++)
            {
                if(z==pb.front || z==(pb.back-1) || y==pb.top|| y==(pb.bottom-1) || x==pb.left || x==(pb.right-1))
				{
					pbptr[x] = 0; // On border, must be zero
                } 
				else
				{
					float val = julia.eval(((float)x/pb.getWidth()-0.5f) * scale, 
							((float)y/pb.getHeight()-0.5f) * scale, 
							((float)z/pb.getDepth()-0.5f) * scale);

					if(val > vcut)
					{
						val = vcut;
					}
					
					Ogre::PixelUtil::packColour((float)x/pb.getWidth(), (float)y/pb.getHeight(), (float)z/pb.getDepth(), (1.0f-(val*vscale))*0.7f, Ogre::PF_A8R8G8B8, &pbptr[x]);
				}	
            }

            pbptr += pb.rowPitch;
        }

        pbptr += pb.getSliceSkip();
    }

	buffer->unlock();

	return trend;
}

void Application::createLight()
{
	m_sceneManager->setAmbientLight(Ogre::ColourValue(0.6, 0.6, 0.6));

	Ogre::Light* l = m_sceneManager->createLight("MainLight");
    l->setDiffuseColour(0.75, 0.75, 0.80);
	l->setSpecularColour(0.9, 0.9, 1);
    l->setPosition(-100,80,50);
	m_sceneManager->getRootSceneNode()->attachObject(l);
}

void Application::createOgreHead()
{
	float height = 100;
	float offset = 20;

	// Ogre head node
	Ogre::SceneNode* fnode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	fnode->setPosition(Ogre::Vector3(0,0,0));
	// Load ogre head
	Ogre::Entity* head = m_sceneManager->createEntity("head", "ogrehead.mesh");
	fnode->attachObject(head);
	// Animation for ogre head
	// Create a track for the light
    Ogre::Animation* anim = m_sceneManager->createAnimation("OgreTrack", 10);
    // Spline it for nice curves
	anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);
    // Create a track to animate the camera's node
    Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, fnode);
    // Setup keyframes
    Ogre::TransformKeyFrame* key = track->createNodeKeyFrame(0); // A startposition
	key->setTranslate(Ogre::Vector3(0.0f, height - offset, 0.0f));
    key = track->createNodeKeyFrame(5);//B
    key->setTranslate(Ogre::Vector3(0.0f, height + offset, 0.0f));
    key = track->createNodeKeyFrame(10);//C
    key->setTranslate(Ogre::Vector3(0.0f, height - offset, 0.0f));
    // Create a new animation state to track this
	mOgreHeadAnimState = m_sceneManager->createAnimationState("OgreTrack");
    mOgreHeadAnimState->setEnabled(true);
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
	if (m_headAnim)
	{
		mOgreHeadAnimState->addTime(elapsedSeconds);
	}
	
	for (int i=0; i<m_numQuadRends; i++)
	{
		static_cast<ThingRenderable*>(m_quadRends[i])->addTime(elapsedSeconds * m_quadSpeedScale);
	}
}

bool Application::keyPressed( const OIS::KeyEvent& e )
{
	if ( e.key == OIS::KC_ESCAPE )
	{
		m_exitRequested = true;
	}

	if ( e.key == OIS::KC_E)
	{
		
	}

	return true;
}

bool Application::mouseMoved( const OIS::MouseEvent& e )
{
	float distanceFactor = 1;
	assert( m_cameraController != NULL );

	bool leftMouseButtonPressed = e.state.buttonDown( OIS::MB_Left );
	if ( leftMouseButtonPressed )
	{
		m_cameraController->addOrientation( -e.state.X.rel, -e.state.Y.rel );
	}
	m_cameraController->addDistance( -e.state.Z.rel * distanceFactor );

	return true;
}