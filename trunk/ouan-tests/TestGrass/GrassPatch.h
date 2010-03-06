
#ifndef	_INCLUDED_GRASSPATCH_H
#define	_INCLUDED_GRASSPATCH_H

             
#include "Ogre.h" 

#include "OgreSimpleRenderable.h"

#include "GrassParticleField.h"
#include "Blade.h"


class GrassPatch : public Ogre::SimpleRenderable
{
public:
	GrassPatch(int nb_blades, int nb_blade_segments, float width, float height, Ogre::SceneManager* ScnMgr);
	~GrassPatch(void);

	void	Update(const Ogre::Vector3 &sphere_center, float sphere_radius, Ogre::Real t);

	friend class	Blade;
   bool     getNormaliseNormals (void) const;  
   bool 	useIdentityProjection (void) const;	
   bool 	useIdentityView (void) const;	

   char iterations[30];
	Blade **m_Blades;
    Ogre::Real getBoundingRadius(void) const;
    Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
	void createSphere(const std::string& strName, const float r, const int nRings, const int nSegments);

protected:
   	void getWorldTransforms(  Ogre::Matrix4* xform ) const;
    /** @copydoc Renderable::getWorldOrientation */
    const  Ogre::Quaternion& getWorldOrientation(void) const;
    /** @copydoc Renderable::getWorldPosition */
    const  Ogre::Vector3& getWorldPosition(void) const;
private:
    Ogre::Mesh*                          mMesh;
    Ogre::Entity*                        mEnt;
    Ogre::SceneNode*                     mSceneNode;
    Ogre::VertexData*                    mvertexData;
    //Ogre::SceneManager*                mSceneMgr;
	Ogre::HardwareVertexBufferSharedPtr	 m_VertexBuffer;
  
    Ogre::Real                           mRadius;

	// Last frame time
	int 						         m_VertexCount;
	int 						         m_TriangleCount;
	GrassParticleField			         *m_Particles;
	int		    				         m_NbBlades;  

};


#endif	/* _INCLUDED_GRASSPATCH_H */