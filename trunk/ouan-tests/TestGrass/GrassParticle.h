
#ifndef	_INCLUDED_GRASS_PARTICLE_H
#define	_INCLUDED_GRASS_PARTICLE_H


#include "Ogre.h"


class GrassParticle
{
public:
	GrassParticle(void);

	// Constraint control
	void	EnableStepping(void);
	void	DisableStepping(void);

	void	SetPosition(const Ogre::Vector3 &pos);
	void	GetPosition(Ogre::Vector3 &dest) const;
	void	SetPositionOnly(const Ogre::Vector3 &pos) { m_Position = pos; }

	void	ClearForce(void);

	void	Integrate(float t);

	friend class GrassParticleField;

private:
	// Current position
	Ogre::Vector3	m_Position;

	// Old position
	Ogre::Vector3	m_OPosition;

	// Acceleration
	Ogre::Vector3	m_Acceleration;

	// Current force affecting this particle
	Ogre::Vector3	m_Force;

	float	m_Mass;

	// Does this particle need stepping?
	bool	m_Step;
};


#endif	/* _INCLUDED_GRASS_PARTICLE_H */