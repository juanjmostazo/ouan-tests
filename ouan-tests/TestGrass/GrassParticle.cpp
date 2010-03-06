#include "GrassParticle.h"

GrassParticle::GrassParticle(void)
{
	// Default settings
	m_Position = Ogre::Vector3::ZERO;
	m_OPosition = Ogre::Vector3::ZERO;
	m_Acceleration = Ogre::Vector3::ZERO;
	m_Force = Ogre::Vector3::ZERO;
	m_Mass = 0.3;
	m_Step = true;
}


void GrassParticle::SetPosition(const Ogre::Vector3 &pos)
{
	m_OPosition = pos;
	m_Position = pos;
}


void GrassParticle::GetPosition(Ogre::Vector3 &dest) const
{
	dest = m_Position;
}


void GrassParticle::EnableStepping(void)
{
	m_Step = true;
}


void GrassParticle::DisableStepping(void)
{
	m_Step = false;
}


void GrassParticle::ClearForce(void)
{
	m_Force = Ogre::Vector3::ZERO;
}


void GrassParticle::Integrate(float t)
{
	Ogre::Vector3	current;

	// Calculate acceleration from the force
	m_Acceleration = m_Force * (1.0f / m_Mass);

	// Backup the current position
	current = m_Position;
	float t_square = t*t;

	// Integrate
	if (((m_Position.y - m_OPosition.y + m_Acceleration.y * t_square)*(m_Position.y - m_OPosition.y + m_Acceleration.y * t_square)<5)&&
		((m_Position.x - m_OPosition.x + m_Acceleration.x * t_square)*(m_Position.x - m_OPosition.x + m_Acceleration.x * t_square)<5)&&
		((m_Position.z - m_OPosition.z + m_Acceleration.z * t_square)*(m_Position.z - m_OPosition.z + m_Acceleration.z * t_square)<5))
	{
		m_Position += m_Position - m_OPosition + m_Acceleration * t_square;
	}
	// Store previous frame position
	m_OPosition = current;
}