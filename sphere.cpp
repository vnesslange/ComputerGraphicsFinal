#include <stdio.h>
#include <math.h>
#include <openGL/gl.h>



#ifndef POINT_H
#define POINT_H

int distort = 0; 

struct point2
{
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

inline
void
DrawPoint( struct point2 *p2 )
{
	glNormal3fv( &p2->nx );
	glTexCoord2fv( &p2->s );
	glVertex3fv( &p2->x );
}
#endif

int		SphNumLngs, SphNumLats;
struct point2 *	SphPts;

inline
struct point2 *
SphPtsPointer( int lat, int lng )
{
	if( lat < 0 )			lat += (SphNumLats-1);
	if( lng < 0 )			lng += (SphNumLngs-0);
	if( lat > SphNumLats-1 )	lat -= (SphNumLats-1);
	if( lng > SphNumLngs-1 )	lng -= (SphNumLngs-0);
	return &SphPts[ SphNumLngs*lat + lng ];
}

void
OsuSphere( float radius, int slices, int stacks, float timer )
{
	// set the globals:

	SphNumLngs = slices;
	SphNumLats = stacks;
	if( SphNumLngs < 3 )
		SphNumLngs = 3;
	if( SphNumLats < 3 )
		SphNumLats = 3;

	// allocate the point data structure:

	SphPts = new struct point2[ SphNumLngs * SphNumLats ];

	// fill the SphPts structure:

	for( int ilat = 0; ilat < SphNumLats; ilat++ )
	{
		float lat = -M_PI/2.  +  M_PI * (float)ilat / (float)(SphNumLats-1);	// ilat=0/lat=0. is the south pole
											// ilat=SphNumLats-1, lat=+M_PI/2. is the north pole
		float xz = cosf( lat );
		float  y = sinf( lat );
		for( int ilng = 0; ilng < SphNumLngs; ilng++ )				// ilng=0, lng=-M_PI and
											// ilng=SphNumLngs-1, lng=+M_PI are the same meridian
		{
			float lng = -M_PI  +  2. * M_PI * (float)ilng / (float)(SphNumLngs-1);
			float x =  xz * cosf( lng );
			float z = -xz * sinf( lng );
			struct point2* p = SphPtsPointer( ilat, ilng );
			p->x  = radius * x;
			p->y  = radius * y;
			p->z  = radius * z;
             if (distort != 1)
            {
			p->nx = x;
			p->ny = y;
			p->nz = z;
           
            p->s = ( lng + M_PI    ) / ( 2.*M_PI );
			p->t = ( lat + M_PI/2. ) / M_PI;

            }
            else
            {
            p->nx = x / timer;
			p->ny = y * timer;
			p->nz = z * lat;
            p->s = (( lng + M_PI + timer ) / ( 2.*M_PI )) * timer;
			p->t = ( (lat - timer )+ M_PI/4. ) / M_PI / timer;

            }
			
		}
	}

	struct point2 top, bot;		// top, bottom points

	top.x =  0.;		top.y  = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s  = 0.;		top.t  = 1.;

	bot.x =  0.;		bot.y  = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s  = 0.;		bot.t  =  0.;

	// connect the north pole to the latitude SphNumLats-2:

	glBegin(GL_TRIANGLE_STRIP);
	for (int ilng = 0; ilng < SphNumLngs; ilng++)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(SphNumLngs - 1);
		top.s = (lng + M_PI) / (2. * M_PI);
		DrawPoint(&top);
		struct point2* p2 = SphPtsPointer(SphNumLats - 2, ilng);	// ilat=SphNumLats-1 is the north pole
		DrawPoint(p2);
	}
	glEnd();

	// connect the south pole to the latitude 1:

	glBegin( GL_TRIANGLE_STRIP );
	for (int ilng = SphNumLngs - 1; ilng >= 0; ilng--)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(SphNumLngs - 1);
		bot.s = (lng + M_PI) / (2. * M_PI);
		DrawPoint(&bot);
		struct point2* p2 = SphPtsPointer(1, ilng);					// ilat=0 is the south pole
		DrawPoint(p2);
	}
	glEnd();

	// connect the horizontal strips:

	for( int ilat = 2; ilat < SphNumLats-1; ilat++ )
	{
		struct point2* p2;
		glBegin(GL_TRIANGLE_STRIP);
		for( int ilng = 0; ilng < SphNumLngs; ilng++ )
		{
			p2 = SphPtsPointer( ilat, ilng );
			DrawPoint( p2 );
			p2 = SphPtsPointer( ilat-1, ilng );
			DrawPoint( p2 );
		}
		glEnd();
	}

	// clean-up:

	delete [ ] SphPts;
	SphPts = NULL;
}