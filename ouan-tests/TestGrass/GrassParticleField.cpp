#include "GrassParticleField.h"


GrassParticleField::GrassParticleField(int nb_grassparticles, float mass)
{
	int		i;

	// Allocate the particle list
	m_NbGrassParticles = nb_grassparticles;
	m_GrassParticles = new GrassParticle[nb_grassparticles];

	// Set the mass of each particle
	for (i = 0; i < m_NbGrassParticles; i++)
		m_GrassParticles[i].m_Mass = mass;
}


GrassParticleField::~GrassParticleField(void)
{
	delete [] m_GrassParticles;
}


void GrassParticleField::Step(float t)
{
	static int ignore = 0;

	for(int i =0; i<m_NbGrassParticles ;i++)
		{
		if (m_GrassParticles[i].m_Step)
		{
		// Add a bit of gravity
			m_GrassParticles[i].m_Force.y -= m_GrassParticles[i].m_Mass * 9.81f;

		// Verlet integration
			m_GrassParticles[i].Integrate(t);
		}
		}

}



void GrassParticleField::ClearForces(void)
{
	int		i;

	for (i = 0; i < m_NbGrassParticles; i++)
		m_GrassParticles[i].ClearForce ();
}


GrassParticle &GrassParticleField::GetGrassParticle(int index)
{
	// Bounds check
	if (index < 0 || index >= m_NbGrassParticles)
	{
		// ERROR
	}

	return (m_GrassParticles[index]);
}