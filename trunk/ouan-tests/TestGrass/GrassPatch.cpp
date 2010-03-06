
#include "GrassPatch.h" 
#include <OgreHardwareBufferManager.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreHardwareIndexBuffer.h>
    

#define POSITION_BINDING 0
#define NORMAL_TEX_BINDING 1
#define COLOR_BINDING 2

using namespace Ogre;
bool GrassPatch::getNormaliseNormals (void) const
{return true;}	
bool  GrassPatch::useIdentityProjection (void) const
{return false;}	
bool  GrassPatch::useIdentityView (void) const
{return false;}	
 void  GrassPatch::getWorldTransforms( Ogre::Matrix4* xform ) const
{
	// return identity matrix to prevent parent transforms
    *xform = Ogre::Matrix4::IDENTITY;
}
//-----------------------------------------------------------------------
const  Ogre::Quaternion& GrassPatch::getWorldOrientation(void) const
{
    return Ogre::Quaternion::IDENTITY;
}
//-----------------------------------------------------------------------
const Ogre::Vector3& GrassPatch::getWorldPosition(void) const
{
    return Ogre::Vector3::ZERO;
}

Ogre::Real GrassPatch::getSquaredViewDepth(const Ogre::Camera* cam) const
	{
		Ogre::Vector3 min, max, mid, dist;

		min = mBox.getMinimum();
		max = mBox.getMaximum();
		mid = ((min - max) * 0.5) + min;
		dist = cam->getDerivedPosition() - mid;
     
		return dist.squaredLength();
         
        // just some value for now 
        //return cam->getDerivedPosition().squaredLength();
	}
Ogre::Real GrassPatch::getBoundingRadius(void) const 
{ 
  return mRadius; 
}  
GrassPatch::GrassPatch(int nb_blades, int nb_blade_segments, float width, float height, Ogre::SceneManager* ScnMgr)
{
	int		i, nb_particles;

	// Create the particle field
	nb_particles = nb_blades * (nb_blade_segments + 1);
	m_Particles = new GrassParticleField(nb_particles, 1);

	// Allocate the blade list
	m_Blades = new Blade *[nb_blades];
	m_NbBlades = nb_blades;
	Ogre::Radian *angle = new Ogre::Radian(Ogre::Math::PI * ((rand()%1000) / 1000.0f));

	// Create the blades of grass
	for (i = 0; i < nb_blades; i++)
	{
		m_Blades[i] = new Blade(m_Particles,
								i * (nb_blade_segments + 1),
								nb_blade_segments,
								4 + ((rand()%1000) / 1000.0f)*0.5f,
								0.3f + ((rand()%1000) / 1000.0f)*0.1f,
								width / 2 - ((rand()%1000) / 500.0f) * width,
								height / 2 - ((rand()%1000) / 500.0f) * height,
								*angle);
		m_Blades[i]->moving = true;
		m_Blades[i]->timeout = 0;
	}
	m_VertexCount = nb_blades * m_Blades[0]->GetStrippedVertexCount();
	m_TriangleCount = ((nb_blade_segments - 1) << 1) + 1;
 
    mvertexData = new Ogre::VertexData ();

    // Vertex declaration
	Ogre::VertexDeclaration* decl = mvertexData->vertexDeclaration;
	Ogre::VertexBufferBinding* binding = mvertexData->vertexBufferBinding;         
    
    size_t offset = 0;

    //position
	decl->addElement (POSITION_BINDING, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION); 
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

    // normal here
	decl->addElement(POSITION_BINDING, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
     
    // Diffuse Color
    decl->addElement(POSITION_BINDING, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);  
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
    
    // texture here
	decl->addElement(POSITION_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);  
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

   //position updated each frame 
	Ogre::HardwareVertexBufferSharedPtr vbuf1 = 
           Ogre::HardwareBufferManager::getSingleton().createVertexBuffer (offset,        // size of one whole vertex
																		   m_VertexCount,                                // number of vertices
																		   Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, // usage
                                                                            false);                                      // no shadow buffer

	binding->setBinding(POSITION_BINDING, vbuf1);
 

	Ogre::HardwareVertexBufferSharedPtr vVertices = binding->getBuffer(POSITION_BINDING);
	Ogre::Real *pVertices = static_cast<Ogre::Real*>( vVertices->lock( Ogre::HardwareBuffer::HBL_DISCARD ) );

    // Colour up the blades
	for (i = 0; i < m_NbBlades; i++)
        m_Blades[i]->Paint (i, pVertices); 
       
	vVertices->unlock();

    // Index buffer to handle triangle strip vertex buffer as triangle list
     
    // Create index buffer 
      int nIndexes = m_TriangleCount*3*m_NbBlades;
   
      Ogre::HardwareIndexBufferSharedPtr iBuf = 
            Ogre::HardwareBufferManager::getSingleton().createIndexBuffer (Ogre::HardwareIndexBuffer::IT_32BIT, 
                                                                nIndexes, 
                                                                Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
                                                                false); 

      // Fill index buffer 
      unsigned int* pIdx = static_cast<unsigned int*> (iBuf->lock (Ogre::HardwareBuffer::HBL_DISCARD)); 
               
       unsigned int j = 0;
        for (i = 0; i < m_NbBlades; i++)
	        m_Blades[i]->UpdateIndexBuffer(&j, i, pIdx);

      iBuf->unlock ();
          

    mRenderOp.vertexData = mvertexData;
//	mRenderOp.vertexData->vertexCount = m_VertexCount;
    mRenderOp.vertexData->vertexCount= nIndexes;
	mRenderOp.vertexData->vertexStart = 0;  

    mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    mRenderOp.indexData = new Ogre::IndexData();
    mRenderOp.indexData->indexBuffer = iBuf; 
    mRenderOp.indexData->indexCount = nIndexes; 
    mRenderOp.indexData->indexStart = 0;  

	mRenderOp.useIndexes = true; 
      
    mRenderOp.srcRenderable = this;

    setMaterial ("Examples/Grass");  
 
    // setup radius the bounding box of this SimpleRenderable to big values to be sure to achieve drawing.
    mRadius = 100;
	setBoundingBox(Ogre::AxisAlignedBox(-300, -300, -300, 600, 600, 600));

 }


GrassPatch::~GrassPatch(void)
{
	int		i;

	for (i = 0; i < m_NbBlades; i++)
		delete m_Blades[i];
	delete [] m_Blades;

	delete m_Particles;
//Ogre  should handle this (or not?)
	delete mRenderOp.vertexData; 
	delete mRenderOp.indexData;
}

void GrassPatch::Update(const Ogre::Vector3 &sphere_center, float sphere_radius, Ogre::Real t)
{
	int			i;
	static  bool inited = false;
	Ogre::Vector3 delta;
	float length;
	int end=0;
	Ogre::Vector3 a, b;
	float radius=sphere_radius*sphere_radius*2.2;

	// Update the particle field
	m_Particles->Step(0.001f); // use of t ?
    Ogre::HardwareVertexBufferSharedPtr vVertices = mRenderOp.vertexData->vertexBufferBinding->getBuffer(POSITION_BINDING);

	Ogre::Real *pVertices = static_cast<Ogre::Real*>( vVertices->lock( 
                                        //Ogre::HardwareBuffer::HBL_DISCARD
                                        Ogre::HardwareBuffer::HBL_NORMAL 
                                            ) );    
  
	// Upload new blade positions
	if(!inited)
	{
		for (i = 0; i < m_NbBlades; i++)
		{
			m_Blades[i]->Constrain(sphere_center, sphere_radius);
			m_Particles->GetGrassParticle(i).ClearForce();
			m_Blades[i]->UpdateVertexBuffer(i, pVertices);
		}
		inited=true;
		vVertices->unlock();
	}
	else
	{	
		for (i = 0; i < m_NbBlades; i++)
		{		
			m_Blades[i]-> GetPosition(a);
			delta = a - sphere_center;
			length = delta.x * delta.x + delta.y * delta.y+ delta.z*delta.z;
			
			if ((m_Blades[i]->moving)||(length-radius<0))
			{
				m_Blades[i]->Constrain(sphere_center, sphere_radius);
				m_Particles->GetGrassParticle(i).ClearForce();
				m_Blades[i]->UpdateVertexBuffer(i, pVertices);
				end++;
			}
		}
		vVertices->unlock();

	}
	int ttt = sprintf(iterations, "Iterations: %d ", end);
	end=0;
}

void GrassPatch::createSphere(const std::string& strName, const float r, const int nRings = 16, const int nSegments = 16)
{
	MeshPtr pSphere = MeshManager::getSingleton().createManual(strName,		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	SubMesh *pSphereVertex = pSphere->createSubMesh();

	pSphere->sharedVertexData = new VertexData();
	VertexData* vertexData = pSphere->sharedVertexData;

	// define the vertex format
	VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
	size_t currOffset = 0;
	// positions
	vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_POSITION);
	currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	// normals
	vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_NORMAL);
	currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	// two dimensional texture coordinates
	vertexDecl->addElement(0, currOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
	currOffset += VertexElement::getTypeSize(VET_FLOAT2);

	// allocate the vertex buffer
	vertexData->vertexCount = (nRings + 1) * (nSegments+1);
	HardwareVertexBufferSharedPtr vBuf = HardwareBufferManager::getSingleton().createVertexBuffer(vertexDecl->getVertexSize(0), vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	VertexBufferBinding* binding = vertexData->vertexBufferBinding;
	binding->setBinding(0, vBuf);
	float* pVertex = static_cast<float*>(vBuf->lock(HardwareBuffer::HBL_DISCARD));

	// allocate index buffer
	pSphereVertex->indexData->indexCount = 6 * nRings * (nSegments + 1);
	pSphereVertex->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT, pSphereVertex->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	HardwareIndexBufferSharedPtr iBuf = pSphereVertex->indexData->indexBuffer;
	unsigned short* pIndices = static_cast<unsigned short*>(iBuf->lock(HardwareBuffer::HBL_DISCARD));

	float fDeltaRingAngle = (Math::PI / nRings);
	float fDeltaSegAngle = (2 * Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			*pVertex++ = x0;
			*pVertex++ = y0;
			*pVertex++ = z0;

			Vector3 vNormal = Vector3(x0, y0, z0).normalisedCopy();
			*pVertex++ = vNormal.x;
			*pVertex++ = vNormal.y;
			*pVertex++ = vNormal.z;

			*pVertex++ = (float) seg / (float) nSegments;
			*pVertex++ = (float) ring / (float) nRings;

			if (ring != nRings) {
                               // each vertex (except the last) has six indices pointing to it
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex;               
				*pIndices++ = wVerticeIndex + nSegments;
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex + 1;
				*pIndices++ = wVerticeIndex;
				wVerticeIndex ++;
			}
		}; // end for seg
	} // end for ring

	// Unlock
	vBuf->unlock();
	iBuf->unlock();
	// Generate face list
	pSphereVertex->useSharedVertices = true;

	// the original code was missing this line:
	pSphere->_setBounds( AxisAlignedBox( Vector3(-r, -r, -r), Vector3(r, r, r) ), false );
	pSphere->_setBoundingSphereRadius(r);
        // this line makes clear the mesh is loaded (avoids memory leaks)
        pSphere->load();
 }
