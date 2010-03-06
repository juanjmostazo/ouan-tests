	
#include "Blade.h"


using namespace Ogre;

Blade::Blade(GrassParticleField *particles_ptr, int particle_index, int nb_segments, float height, float width, float xpos, float ypos, Ogre::Radian angle_y)
{
	int		i;

	assert(nb_segments < MAX_NB_SEGMENTS);

	// Setup blade fields
	m_Particles = particles_ptr;
	m_StartParticle = particle_index;
	m_NbParticles = nb_segments + 1;


    m_Orientation.FromAxisAngle (Ogre::Vector3::UNIT_X,  angle_y); 
    m_Orientation.FromAxisAngle (Ogre::Vector3::UNIT_Y,  angle_y);
 
	m_SegmentLength = height / (m_NbParticles - 1);

	// Pre-calc the grass width at various heights
	for (i = 0; i < m_NbParticles; i++)
	{
		Ogre::Vector3 twidth = Ogre::Vector3 (width - i * (width / (m_NbParticles - 1)), 0, 0);
        m_Widths[i] =  m_Orientation * twidth;
       
	}

	// Disable the ground particle from being stepped (cheap hinge)
	m_Particles->GetGrassParticle (m_StartParticle).DisableStepping();
	m_Particles->GetGrassParticle (m_StartParticle).SetPosition(Ogre::Vector3 (xpos, 0, ypos));
	m_Position.x=xpos;
	m_Position.z=ypos;
	m_Position.y=0.0f;

	// Set the initial position of all particles
	for (i = 1; i < m_NbParticles; i++)
	{
		int minus;
		if(rand()%2==1)
			{
			minus=-1;
			}
		else minus = 1;
		m_Particles->GetGrassParticle (m_StartParticle + i).SetPosition(Ogre::Vector3 (xpos+minus*((rand()%10) / 10.0f)*2 , i * m_SegmentLength, ypos + (minus*((rand()%10) / 10.0f)*2)));
	}
}


Blade::~Blade(void)
{
}     

	void Blade::GetPosition(Ogre::Vector3 &pos)
	{
		pos=m_Position;
	}
 void Blade::UpdateVertexBuffer(int index, Ogre::Real *pVertices)
{
	int		        i, j, start, end, p_start, bufferindex;
	Ogre::Vector3	p, tempvec;


	// Get this blade's section within the vertex buffer
	start = index * (((m_NbParticles - 1) << 1) + 1); 
	end = start + ((m_NbParticles - 1) << 1);
	
	p_start = index * m_NbParticles;
 
    bufferindex = start * 9;
	for (i = start, j = 0; i < end; i += 2, j++)
	{
		// Get particle position that influences the next two vertex positions
		m_Particles->GetGrassParticle(p_start + j).GetPosition(p);

		// Set the positions either side of the particle
        tempvec = p - m_Widths[j];
		pVertices[bufferindex + 0] = static_cast<Ogre::Real> (tempvec.x);
        pVertices[bufferindex + 1] = static_cast<Ogre::Real> (tempvec.y);
        pVertices[bufferindex + 2] = static_cast<Ogre::Real> (tempvec.z); 

        bufferindex += 9;

	    tempvec = p + m_Widths[j];
		pVertices[bufferindex + 0] = static_cast<Ogre::Real> (tempvec.x);
        pVertices[bufferindex + 1] = static_cast<Ogre::Real> (tempvec.y);
        pVertices[bufferindex + 2] = static_cast<Ogre::Real> (tempvec.z);  
        bufferindex += 9;
	}
	// Set the position of the top-most vertex
    m_Particles->GetGrassParticle(p_start + j).GetPosition(p);
	pVertices[bufferindex + 0] = static_cast<Ogre::Real> (p.x);
    pVertices[bufferindex + 1] = static_cast<Ogre::Real> (p.y);
    pVertices[bufferindex + 2] = static_cast<Ogre::Real> (p.z); 
}

 void Blade::Paint ( int index, 
                    Ogre::Real *pVertices)
{
	int		i, j, start, end, bufferindex; 
    Ogre::ColourValue v;
    Ogre::RGBA colour;

	// Get this blade's section within the vertex buffer
	start = index * (((m_NbParticles - 1) << 1) + 1);
	end = start + ((m_NbParticles - 1) << 1);

	long level = 128 + rand()%128;  
    v.r = level; v.g = level; v.b = level; 
    Ogre::Root::getSingleton().convertColourValue (v, &colour);
    //pVertices += start * (3+3+1+2) * sizeof (Ogre::Real);
		for (i = start, j = 0; i < end; i += 2, j++)
	{  
		bufferindex = i * 9;
            // Bogus normal
        pVertices[bufferindex + 3] = static_cast<Ogre::Real> (0.0f);
        pVertices[bufferindex + 4] = static_cast<Ogre::Real> (0.0f);
        pVertices[bufferindex + 5] = static_cast<Ogre::Real> (-1.0f); 

		    // Diffuse reflectivity
        pVertices[bufferindex + 6] = static_cast<Ogre::Real> (colour);

             // Texture co-ordinates               
        pVertices[bufferindex + 7] = static_cast<Ogre::Real> (j * (1.0f / (m_NbParticles - 1)));
        pVertices[bufferindex + 8] = static_cast<Ogre::Real> (0.0f);

       //2nd
       		// Bogus normal
        pVertices[bufferindex+9 + 3] = static_cast<Ogre::Real> (0.0f);
        pVertices[bufferindex+9 + 4] = static_cast<Ogre::Real> (0.0f);
        pVertices[bufferindex+9 + 5] = static_cast<Ogre::Real> (-1.0f); 

		    // Diffuse reflectivity
        //pVertices[bufferindex + 6] = static_cast<Ogre::Real> (colour);

             // Texture co-ordinates               
        pVertices[bufferindex+9 + 7] = static_cast<Ogre::Real> (j * (1.0f / (m_NbParticles - 1)));
        pVertices[bufferindex+9 + 8] = static_cast<Ogre::Real> (1.0f);
	}  
	// And the last vertex 
        // Bogus normal
    pVertices[bufferindex + 3] = static_cast<Ogre::Real> (0.0f);
    pVertices[bufferindex + 4] = static_cast<Ogre::Real> (0.0f);
    pVertices[bufferindex + 5] = static_cast<Ogre::Real> (-1.0f); 

        // Diffuse reflectivity
    pVertices[bufferindex + 6] = static_cast<Ogre::Real> (0xFFFFFFFF);

        // Texture co-ordinates               
    pVertices[bufferindex + 7] = static_cast<Ogre::Real> (0.99f);
    pVertices[bufferindex + 8] = static_cast<Ogre::Real> (0.5f);
} 
void Blade::Constrain(const Ogre::Vector3 &sphere_center, float sphere_radius)
{
	int		i, j, end;
	Ogre::Vector3	a, b, last_up, up, delta, delta2, p;
	float	length, diff;

	// Try to stop blade->sphere penetration
	sphere_radius += 0.05f;

	// Calc the last particle affected
	end = m_StartParticle + m_NbParticles;

	// Spine should be perpendicular to the grass patch
	last_up = Ogre::Vector3 (0, 1, 0);

	for (i = m_StartParticle + 1, j = 0; i < end; i++, j++)
	{
		// Get the two particles comprising this segment
		m_Particles->GetGrassParticle(i).GetPosition(a);
		m_Particles->GetGrassParticle(i - 1).GetPosition(b);

		// Constrain the particle to the ground plane
		if (a.y < 0)
			a.y = 0.1;
		timeout++;
		// Constrain to stick-like grass spine
		if (timeout > 2000)
		{
			timeout = 0;
			moving = false;
		}
		else
		{
			delta = a - b;
			length = delta.length (); 
			diff = length - m_SegmentLength;
			a -= delta * (0.5*diff/ length);

			// Get the current blade segment up vector

			up = delta.normalisedCopy ();

			// Get the vector between the two particles in the constraint and project onto the constraint plane
			p = delta;
			//p += last_up * -D3DXVec3Dot(&p, &last_up);
			p += last_up * - p.dotProduct (last_up);

			// Apply the spring constraint (translational spring - rotation provided by above spine constraint: saves lots of cycles)
			length = p.length ();
			diff = (length - 0.1f) / length;   
			a -= p * diff;
		}
		// Check for intersection with the sphere
		delta = a - sphere_center;
		length = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
		if (length - sphere_radius * sphere_radius < 0)
		{
			// Get sphere contact normal
			length = (float) sqrt (length);
			delta.x /= length;
			delta.y /= length;
			delta.z /= length;

			// Project particle out of the sphere
			a = sphere_center + delta * sphere_radius;
			moving = true;
		}

		// Get the new up vector
		delta = a - b;
		length = (float) sqrt (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
		up.x = delta.x / length;
		up.y = delta.y / length;
		up.z = delta.z / length;

		// Is this part of the blade broken?
		if (up.dotProduct(last_up) < 1e-2)
		{
			// Get the target position
			Ogre::Vector3 p = b + last_up * length;

			// Interpolate a little towards it to fix the blade
			a += (p - a) * 0.4f;
		}
		m_Particles->GetGrassParticle(i).SetPositionOnly(a);
		last_up = up;
		// Set the new position and record the previous up vector
	}
}



void Blade::UpdateIndexBuffer (unsigned int *indice_index, unsigned int vertex_index,  unsigned int *vptr)
{
	unsigned int		j, i, start, end;

    j =  *indice_index;
	// Get this blade's section within the vertex buffer
	start = vertex_index * (((m_NbParticles - 1) << 1) + 1);
	end = start + ((m_NbParticles - 1) << 1);
    end = end - 1;
	for (i = start; i < end; i++)
	{
		vptr[j++] = i; 
		vptr[j++] = (i + 1);
		vptr[j++] = (i + 2);
	}
    *indice_index = j;
}

int Blade::GetStrippedVertexCount(void)
{
	return (((m_NbParticles - 1) << 1) + 1);
}

