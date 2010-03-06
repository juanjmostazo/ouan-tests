
#ifndef	_INCLUDED_BLADE_H
#define	_INCLUDED_BLADE_H

#include "Ogre.h"  
#include "GrassParticleField.h"

class Blade
{
public:


	Blade(GrassParticleField *particles_ptr, int particle_index, int nb_segments, float height, float width, float xpos, float ypos, Ogre::Radian angle_y);
	~Blade(void);

	char texs[20];
	int timeout;
	bool moving;

    // Texture and colour the blade of grass
	void Blade::GetPosition(Ogre::Vector3 &pos);
	void Blade::Paint ( int index, 
                        Ogre::Real *pVertices);

	// Act to constrain all the particles of this blade
	void Constrain(const Ogre::Vector3 &sphere_center, float sphere_radius);

	// Writes this blade's data to a vertex buffer
	void	UpdateVertexBuffer(int index,Ogre::Real *pVertices);
         
	// Reference this blade's vertex position to index buffer
    void    UpdateIndexBuffer(unsigned int *indice_index, unsigned int vertex_index, unsigned int *vptr);

	// How many vertices are required to make a triangle strip of this blade
	int		GetStrippedVertexCount(void);

	const Ogre::Matrix3& GetOrientation(void) const { return (m_Orientation); }
private:
	// Particle field used to control this blade of grass
	GrassParticleField	*m_Particles;

	// Starting particle index within the particle field
	int				m_StartParticle;

	// Number of particle used to define this blade of grass
	int				m_NbParticles;

	Ogre::Matrix3			m_Orientation;

	// Grass widths at various heights
	enum			{ MAX_NB_SEGMENTS = 10 };
	Ogre::Vector3			m_Widths[MAX_NB_SEGMENTS + 1];

	// Normals for each segment
	Ogre::Vector3			m_Normals[MAX_NB_SEGMENTS];

	// Natural spring length of each segment
	float			m_SegmentLength;
	Ogre::Vector3			m_Position;
};


#endif	/* _INCLUDED_BLADE_H */