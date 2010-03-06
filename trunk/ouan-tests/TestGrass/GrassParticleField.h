
#ifndef	_INCLUDED_GRASS_PARTICLEFIELD_H
#define	_INCLUDED_GRASS_PARTICLEFIELD_H

#include "Ogre.h"  
#include "GrassParticle.h"
         

class GrassParticleField
{
public:
	GrassParticleField(int nb_grassparticles, float mass);
	~GrassParticleField(void);

	// Integrate and constrain all particles in this field
	void		Step(float t);

	void		ClearForces(void);

	GrassParticle	&GetGrassParticle(int index);

private:
	// Total particle count
	int			m_NbGrassParticles;

	// All particles
	GrassParticle	*m_GrassParticles;
};


#endif	/* _INCLUDED_GRASS_PARTICLEFIELD_H */