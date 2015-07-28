/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Lattice is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Lattice is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Lattice screensaver

#include "Lattice.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "texture.h"

#include "camera.h"
#include "resource.h"

// Globals

static int transitions[120] = {
        1, 2, 12, 4, 14, 8,
        0, 3, 15, 7, 7, 7,
        3, 4, 14, 0, 7, 16,
        2, 1, 15, 7, 7, 7,
        5, 10, 12, 17, 17, 17,
        4, 3, 13, 11, 9, 17,
        12, 4, 10, 17, 17, 17,
        2, 0, 14, 8, 16, 19,
        1, 3, 15, 7, 7, 7,
        4, 10, 12, 17, 17, 17,
        11, 4, 12, 17, 17, 17,
        10, 5, 15, 13, 17, 18,
        13, 10, 4, 17, 17, 17,
        12, 1, 11, 5, 6, 17,
        15, 2, 12, 0, 7, 19,
        14, 3, 1, 7, 7, 7,
        3, 1, 15, 7, 7, 7,
        5, 11, 13, 6, 9, 18,
        10, 4, 12, 17, 17, 17,
        15, 1, 3, 7, 7, 7};

// Modulus function for picking the correct element of lattice array
int myMod(int x)
{
	while(x < 0)
		x += LATSIZE;
	return(x % LATSIZE);
}


// start point, start slope, end point, end slope, position (0.0 - 1.0)
// returns point somewhere along a smooth curve between the start point
// and end point

float interpolate(float a, float b, float c, float d, float where)
{
	float q = 2.0f * (a - c) + b + d;
	float r = 3.0f * (c - a) - 2.0f * b - d;
    
	return((where * where * where * q) + (where * where * r) + (where * b) + a);
}

scene::scene()
{
	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	
	_path=(float **) malloc(7*sizeof(float **));
	
	for(int i=0;i<7;i++)
	{
		_path[i]=(float *) malloc(6*sizeof(float));
	}
	
	_bPnt=(float **) malloc(10*sizeof(float **));
	
	for(int i=0;i<10;i++)
	{
		_bPnt[i]=(float *) malloc(6*sizeof(float));
	}
	
	_flymode = 1;
	
	_rollVel = 0.0f;
	_rollAcc = 0.0f;
	
	_rollChange = rsRandf(10.0f) + 2.0f;
	
	_oldxyz=rsVec(0.0f, 0.0f, 0.0f);
	_oldDir=rsVec(0.0f, 0.0f, -1.0f);
	_oldAngvel=rsVec(0.0f, 0.0f, 0.0f);
	
	_quat=rsQuat();
	
	_where=0.0;
	
	_segments=0;
	_seg=0;
	
	_flymodeChange = 20.0f;
	
	_camera=NULL;
}

scene::~scene()
{
	if (_camera!=NULL)
	{
		delete _camera;
		_camera=NULL;
	}
	
	for(int i=0;i<7;i++)
	{
		free(_path[i]);
	}
	
	free(_path);
	
	for(int i=0;i<10;i++)
	{
		free(_bPnt[i]);
	}
	
	free(_bPnt);
}

#pragma mark -

void scene::initTextures(void)
{
	switch(torusTexture)
    {
        case LATTICE_TEXTURE_INDUSTRIAL:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,GL_UNSIGNED_BYTE, indtex1);
    
            glBindTexture(GL_TEXTURE_2D, 2);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,GL_UNSIGNED_BYTE, indtex2);
            break;
        case LATTICE_TEXTURE_CRYSTAL:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,GL_UNSIGNED_BYTE, crystex);
            break;
        case LATTICE_TEXTURE_CHROME:
        	glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,GL_UNSIGNED_BYTE, chrometex);
            break;
        case LATTICE_TEXTURE_BRASS:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,GL_UNSIGNED_BYTE, brasstex);
            break;
		case LATTICE_TEXTURE_SHINY:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 4, TEXSIZE, TEXSIZE, GL_RGBA,GL_UNSIGNED_BYTE, shinytex);
            break;
        case LATTICE_TEXTURE_GHOSTLY:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, TEXSIZE, TEXSIZE, GL_ALPHA,GL_UNSIGNED_BYTE, ghostlytex);
            break;
        case LATTICE_TEXTURE_CIRCUITS:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, TEXSIZE, TEXSIZE, GL_ALPHA,GL_UNSIGNED_BYTE, circuittex);
            break;
        case LATTICE_TEXTURE_DOUGHNUTS:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 4, TEXSIZE, TEXSIZE, GL_RGBA,GL_UNSIGNED_BYTE, doughtex);
            break;
    }
}


void scene::makeTorus(int smooth, int longitude, int latitude, float centerradius, float thickradius)
{
	int i, j;
	float r, rr;  // Radius
	float z, zz;  // Depth
	float cosa, sina;  // Longitudinal positions
	float cosn, cosnn, sinn, sinnn;  // Normals for shading
	float ncosa, nsina;  // Longitudinal positions for shading
	float u, v1, v2, ustep, vstep;
	float temp;
	float oldcosa=0., oldsina=0., oldncosa=0., oldnsina=0., oldcosn=0., oldcosnn=0., oldsinn=0., oldsinnn=0.;

	// Smooth shading?
	if(smooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	// Initialize texture stuff
	vstep = 1.0f / float(latitude);
	ustep = float(int((centerradius / thickradius) + 0.5f)) / float(longitude);
	v2 = 0.0f;

	for(i=0; i<latitude; i++)
	{
		temp = PIx2 * float(i) / float(latitude);
		cosn = (float) cos(temp);
		sinn = (float) sin(temp);
		temp = PIx2 * float(i+1) / float(latitude);
		cosnn = (float) cos(temp);
		sinnn = (float) sin(temp);
		r = centerradius + thickradius * cosn;
		rr = centerradius + thickradius * cosnn;
		z = thickradius * sinn;
		zz = thickradius * sinnn;
		if(!smooth){  // Redefine normals for flat shaded model
			temp = PIx2 * (float(i) + 0.5f) / float(latitude);
			cosn = cosnn = (float) cos(temp);
			sinn = sinnn = (float) sin(temp);
		}
		v1 = v2;
		v2 += vstep;
		u = 0.0f;
		glBegin(GL_TRIANGLE_STRIP);
			for(j=0; j<longitude; j++)
			{
				temp = PIx2 * float(j) / float(longitude);
				cosa = (float) cos(temp);
				sina = (float) sin(temp);
				if(smooth)
				{
					ncosa = cosa;
					nsina = sina;
				}
				else
				{  // Redefine longitudinal component of normal for flat shading
					temp = PIx2 * (float(j) - 0.5f) / float(longitude);
					ncosa = (float) cos(temp);
					nsina = (float) sin(temp);
				}
				if(j==0)
				{  // Save first values for end of circular tri-strip
					oldcosa = cosa;
					oldsina = sina;
					oldncosa = ncosa;
					oldnsina = nsina;
					oldcosn = cosn;
					oldcosnn = cosnn;
					oldsinn = sinn;
					oldsinnn = sinnn;
				}
				glNormal3f(cosnn * ncosa, cosnn * nsina, sinnn);
				glTexCoord2f(u, v2);
				glVertex3f(cosa * rr, sina * rr, zz);
				glNormal3f(cosn * ncosa, cosn * nsina, sinn);
				glTexCoord2f(u, v1);
				glVertex3f(cosa * r, sina * r, z);
				u += ustep;  // update u texture coordinate
			}
			//  Finish off circular tri-strip with saved first values
			glNormal3f(oldcosnn * oldncosa, oldcosnn * oldnsina, oldsinnn);
			glTexCoord2f(u, v2);
			glVertex3f(oldcosa * rr, oldsina * rr, zz);
			glNormal3f(oldcosn * oldncosa, oldcosn * oldnsina, oldsinn);
			glTexCoord2f(u, v1);
			glVertex3f(oldcosa * r, oldsina * r, z);
		glEnd();
	}
}


//  Build the lattice display lists
void scene::makeLatticeObjects(void)
{
	int i, d = 0;
	float thick = float(torusThickness) * 0.001f;

	if(torusTexture > LATTICE_TEXTURE_NONE && torusTexture < LATTICE_TEXTURE_DOUGHNUTS)
		glColor3f(1.0f, 1.0f, 1.0f);

	for(i=1; i<=NUMOBJECTS; i++)
    {
		glNewList(i, GL_COMPILE);
			if(torusTexture >= LATTICE_TEXTURE_CRYSTAL)
				glBindTexture(GL_TEXTURE_2D, 1);
                
			if(d < density)
            {
				glPushMatrix();
					if(torusTexture == LATTICE_TEXTURE_NONE || torusTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
					else
                    if(torusTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, rsRandi(2)+1);
					glTranslatef(-0.25f, -0.25f, -0.25f);
					if(rsRandi(2))
						glRotatef(180.0f, 1, 0, 0);
					scene::makeTorus(dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < density)
            {
				glPushMatrix();
					if(torusTexture == LATTICE_TEXTURE_NONE || torusTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
					else
                    if(torusTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, rsRandi(2)+1);
					glTranslatef(0.25f, -0.25f, -0.25f);
					if(rsRandi(2))
						glRotatef(90.0f, 1, 0, 0);
					else
						glRotatef(-90.0f, 1, 0, 0);
					scene::makeTorus(dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < density)
            {
				glPushMatrix();
					if(torusTexture == LATTICE_TEXTURE_NONE || torusTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
					else
                    if(torusTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, rsRandi(2)+1);
					glTranslatef(0.25f, -0.25f, 0.25f);
					if(rsRandi(2))
						glRotatef(90.0f, 0, 1, 0);
					else
						glRotatef(-90.0f, 0, 1, 0);
					scene::makeTorus(dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < density)
            {
				glPushMatrix();
					if(torusTexture == LATTICE_TEXTURE_NONE || torusTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
					else
                    if(torusTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, rsRandi(2)+1);
					glTranslatef(0.25f, 0.25f, 0.25f);
					if(rsRandi(2))
						glRotatef(180.0f, 1, 0, 0);
					scene::makeTorus(dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < density)
            {
				glPushMatrix();
					if(torusTexture == LATTICE_TEXTURE_NONE || torusTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
					else
                    if(torusTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, rsRandi(2)+1);
					glTranslatef(-0.25f, 0.25f, 0.25f);
					if(rsRandi(2))
						glRotatef(90.0f, 1, 0, 0);
					else
						glRotatef(-90.0f, 1, 0, 0);
					scene::makeTorus(dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < density)
            {
				glPushMatrix();
					if(torusTexture == LATTICE_TEXTURE_NONE || torusTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
					else
                    if(torusTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, rsRandi(2)+1);
					glTranslatef(-0.25f, 0.25f, -0.25f);
					if(rsRandi(2))
						glRotatef(90.0f, 0, 1, 0);
					else
						glRotatef(-90.0f, 0, 1, 0);
					scene::makeTorus(dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
		glEndList();
		d = (d + 37) % 100;
	}
}

void scene::reconfigure()
{
	int i, j;
	int newBorder, positive;
    float **path=_path;
    int * globalxyz=_globalxyz;
    
	// End of old path = start of new path
	for(i=0; i<6; i++)
		path[0][i] = path[_segments][i];

	// determine if direction of motion is positive or negative
	// update global position
	if(_lastBorder < 6)
    {
		if((path[0][3] + path[0][4] + path[0][5]) > 0.0f)
        {
			positive = 1;
			globalxyz[_lastBorder / 2] ++;
		}
		else
        {
			positive = 0;
			globalxyz[_lastBorder / 2] --;
		}
	}
	else
    {
		if(path[0][3] > 0.0f)
        {
			positive = 1;
			globalxyz[0] ++;
		}
		else
        {
			positive = 0;
			globalxyz[0] --;
		}
        
		if(path[0][4] > 0.0f)
			globalxyz[1] ++;
		else
			globalxyz[1] --;
		if(path[0][5] > 0.0f)
			globalxyz[2] ++;
		else
			globalxyz[2] --;
	}

	if(!rsRandi(11 - dPathrand))
    {  // Change directions
		if(!positive)
			_lastBorder += 10;
		newBorder = transitions[_lastBorder*6+rsRandi(6)];
		positive = 0;
		if(newBorder < 10)
			positive = 1;
		else
			newBorder -= 10;
		for(i=0; i<6; i++)  // set the new border point
			path[1][i] = _bPnt[newBorder][i];
		if(!positive)
		{  // flip everything if direction is negative
			if(newBorder < 6)
				path[1][newBorder/2] *= -1.0f;
			else
				for(i=0; i<3; i++)
					path[1][i] *= -1.0f;
			for(i=3; i<6; i++)	
				path[1][i] *= -1.0f;
		}
		for(i=0; i<3; i++)  // reposition the new border
			path[1][i] += globalxyz[i];
		_lastBorder = newBorder;
		_segments = 1;
	}
	else
    {  // Just keep going straight
		newBorder = _lastBorder;
		for(i=0; i<6; i++)
			_path[1][i] = _bPnt[newBorder][i];
		i = newBorder / 2;
		if(!positive)
        {
			if(newBorder < 6)
				_path[1][i] *= -1.0f;
			else
			{
				_path[1][0] *= -1.0f;
				_path[1][1] *= -1.0f;
				_path[1][2] *= -1.0f;
			}
			_path[1][3] *= -1.0f;
			_path[1][4] *= -1.0f;
			_path[1][5] *= -1.0f;
		}
		for(j=0; j<3; j++)
        {
			_path[1][j] += globalxyz[j];
			if((newBorder < 6) && (j != 1))
				path[1][j] += rsRandf(0.15f) - 0.075f;
		}
		if(newBorder >= 6)
			path[1][0] += rsRandf(0.1f) - 0.05f;
		_segments = 1;
	}
}


void scene::draw(void)
{
	struct timeval tTime;
	unsigned long long tCurentTime;
	
	gettimeofday(&tTime, NULL);
	
	tCurentTime=(tTime.tv_sec*1000+tTime.tv_usec/1000);
	
	float tElapsedTime=(tCurentTime-_lastRefresh)*0.001;
	_lastRefresh=tCurentTime;
	
	int i, j, k;
	int indexx, indexy, indexz;
	rsVec xyz, dir, angvel, tempVec;
	float angle, distance;
	float rotationInertia = 0.007f * float(dSpeed);
//static float rotationInertia = 0.00014f * float(dSpeed);
	 float maxSpin = 0.0025f * float(dSpeed);
//static float maxSpin = 0.00005f * float(dSpeed);
	float rotMat[16];
	rsQuat newQuat;
	//float normScale;
	 
	//rsQuat cullQuat;
	float cullMat[16];
	float transMat[16];
    float ** path=_path;
	//rsVec cull[5];  // storage for transformed culling vectors
	//static int drawDepth = (1.0f>sinf(float(dFov)*0.5f*D2R)/cosf(float(dFov)*0.5f*D2R))
	//	?dDepth
	//	:int(float(dDepth)*(sinf(float(dFov)*0.5f*D2R)/cosf(float(dFov)*0.5f*D2R))+0.5f);
	 int drawDepth = dDepth + 2;

	_where += float(dSpeed) * 0.05f * tElapsedTime;
//where += float(dSpeed) * 0.001f * tElapsedTime;
	if(_where >= 1.0f)
    {
		_where -= 1.0f;
		_seg++;
	}
    
	if(_seg >= _segments)
    {
		_seg = 0;
		reconfigure();
	}
    
    int seg=_seg;
    
	// Calculate position
	xyz[0] = interpolate(path[seg][0], path[seg][3], path[seg+1][0],
		path[seg+1][3], _where);
	xyz[1] = interpolate(path[seg][1], path[seg][4], path[seg+1][1],
		path[seg+1][4], _where);
	xyz[2] = interpolate(path[seg][2], path[seg][5], path[seg+1][2],
		path[seg+1][5], _where);

	// Do rotation stuff
	dir = xyz - _oldxyz;  // Direction of motion
	dir.normalize();
	angvel.cross(dir, _oldDir);  // Desired axis of rotation
	float dot = _oldDir.dot(dir);
	if(dot < -1.0f)
		dot = -1.0f;
	if(dot > 1.0f)
		dot = 1.0f;
	
	angle = (float) acos(dot);  // Desired turn angle
	if(angle > maxSpin)  // Cap the spin
		angle = maxSpin;
	if(angle < -maxSpin)
		angle = -maxSpin;
		
	angvel.scale(angle);  // Desired angular velocity
	tempVec = angvel - _oldAngvel;  // Change in angular velocity
	distance = tempVec.length();  // Don't let angular velocity change too much
	if(distance > rotationInertia * tElapsedTime)
    {
		tempVec.scale((rotationInertia * tElapsedTime) / distance);
		angvel = _oldAngvel + tempVec;
	}
    
	_flymodeChange -= tElapsedTime;
	
	// transition from one fly mode to the other?
	if(_flymodeChange <= 1.0f)  // prepare to transition
		angvel.scale(_flymodeChange);
        
	if(_flymodeChange <= 0.0f)
    {  // transition from one fly mode to the other?
		_flymode = rsRandi(4);
		_flymodeChange = rsRandf(float(150 - dSpeed)) + 5.0f;
	}
    
	tempVec = angvel;  // Recompute desired rotation
	angle = tempVec.normalize();
	newQuat.make(angle, tempVec[0], tempVec[1], tempVec[2]);  // Use rotation	
	if(_flymode)  // fly normal (straight)
		_quat.preMult(newQuat);
	else  // don't fly normal (go backwards and stuff)
		_quat.postMult(newQuat);

	// Roll
	
	_rollChange -= tElapsedTime;
    
	if(_rollChange <= 0.0f)
    {
		_rollAcc = rsRandf(0.02f * float(dSpeed)) - (0.01f * float(dSpeed));
		_rollChange = rsRandf(10.0f) + 2.0f;
	}
    
	_rollVel += _rollAcc * tElapsedTime;
    
	if(_rollVel > (0.04f * float(dSpeed)) && _rollAcc > 0.0f)
		_rollAcc = 0.0f;
	if(_rollVel < (-0.04f * float(dSpeed)) && _rollAcc < 0.0f)
		_rollAcc = 0.0f;
    
    newQuat.make(_rollVel * tElapsedTime, _oldDir[0], _oldDir[1], _oldDir[2]);
	_quat.preMult(newQuat);

	_quat.toMat(rotMat);

	// Save old stuff
	_oldxyz = xyz;
	_oldDir[0] = -rotMat[2];
	_oldDir[1] = -rotMat[6];
	_oldDir[2] = -rotMat[10];
	_oldAngvel = angvel;

	// Apply transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(rotMat);
	glGetFloatv(GL_MODELVIEW, cullMat);
	glTranslatef(-xyz[0], -xyz[1], -xyz[2]);
	glGetFloatv(GL_MODELVIEW, transMat);
	
	// Just in case display lists contain no colors
	glColor3f(1.0f, 1.0f, 1.0f);
	
	// Environment mapping for crystal, chrome, brass, shiny, and ghostly
	if(torusTexture == LATTICE_TEXTURE_CRYSTAL ||
	   torusTexture == LATTICE_TEXTURE_CHROME ||
	   torusTexture == LATTICE_TEXTURE_BRASS  ||
	   torusTexture == LATTICE_TEXTURE_SHINY ||
	   torusTexture == LATTICE_TEXTURE_GHOSTLY)
	{
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	}

	// Render everything
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	for(i=_globalxyz[0]-drawDepth; i<=_globalxyz[0]+drawDepth; i++)
    {
		for(j=_globalxyz[1]-drawDepth; j<=_globalxyz[1]+drawDepth; j++)
        {
			for(k=_globalxyz[2]-drawDepth; k<=_globalxyz[2]+drawDepth; k++)
            {
				tempVec[0] = i - xyz[0];
				tempVec[1] = j - xyz[1];
				tempVec[2] = k - xyz[2];
                
				float tpos[3];  // transformed position
				tpos[0] = tempVec[0]*rotMat[0] + tempVec[1]*rotMat[4] + tempVec[2]*rotMat[8];// + rotMat[12];
				tpos[1] = tempVec[0]*rotMat[1] + tempVec[1]*rotMat[5] + tempVec[2]*rotMat[9];// + rotMat[13];
				tpos[2] = tempVec[0]*rotMat[2] + tempVec[1]*rotMat[6] + tempVec[2]*rotMat[10];// + rotMat[14];
				
                if(_camera->inViewVolume(tpos, 0.9f))
                {
					indexx = myMod(i);
					indexy = myMod(j);
					indexz = myMod(k);
					// draw it
					glPushMatrix();
						glTranslatef((float)i, (float)j, (float)k);
						glCallList(_lattice[indexx][indexy][indexz]);
					glPopMatrix();
				}
			}
		}
	}
	
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

void scene::create()
{
	int i, j, k;
	
	if(torusTexture != LATTICE_TEXTURE_CHROME && torusTexture != LATTICE_TEXTURE_BRASS && torusTexture != LATTICE_TEXTURE_GHOSTLY)
	{  // No lighting for chrome, brass, or ghostly
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		float ambient[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		float diffuse[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float specular[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float position[4] = {400.0f, 300.0f, 500.0f, 0.0f};
		//float position[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	if(torusTexture == LATTICE_TEXTURE_NONE  ||
	   torusTexture == LATTICE_TEXTURE_SHINY ||
	   torusTexture >= LATTICE_TEXTURE_CIRCUITS)
	{
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
		glColorMaterial(GL_FRONT, GL_SPECULAR);
	}
	
	if(torusTexture == LATTICE_TEXTURE_CRYSTAL)
	{
		glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);
		glColorMaterial(GL_FRONT, GL_SPECULAR);
	}
	
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	if(dFog)
	{
		glEnable(GL_FOG);
		float fog_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		glFogfv(GL_FOG_COLOR, fog_color);
		glFogf(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, float(dDepth) * 0.3f);
		glFogf(GL_FOG_END, float(dDepth) - 0.1f);
	}
	else
	{
		glDisable(GL_FOG);
	}
	
	glDisable(GL_BLEND);
	
	if(torusTexture == LATTICE_TEXTURE_CRYSTAL || torusTexture == LATTICE_TEXTURE_GHOSTLY)
	{  // Use blending for crystal and ghostly
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
	}
	
	if(torusTexture == LATTICE_TEXTURE_CIRCUITS)
	{  // Use blending for circuits
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}
	
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if(torusTexture)
	{
		glEnable(GL_TEXTURE_2D);
		initTextures();
		// Environment mapping for crystal, chrome, brass, shiny, and ghostly
		if(torusTexture >= LATTICE_TEXTURE_CRYSTAL && torusTexture <= LATTICE_TEXTURE_GHOSTLY)
		{
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		}
	}
	
	// Initialize lattice objects and their positions in the lattice array
	makeLatticeObjects();
	for(i=0; i<LATSIZE; i++)
	{
		for(j=0; j<LATSIZE; j++)
		{
			for(k=0; k<LATSIZE; k++)
			{
				_lattice[i][j][k] = rsRandi(NUMOBJECTS) + 1;
			}
		}
	}
	
	// Initialize border points
	// horizontal border points
	for(i=0; i<6; i++)
	{
		for(j=0; j<6; j++)
		{
			_bPnt[i][j] = 0.0f;
		}
	}
	
	_bPnt[0][0] = 0.5f;    _bPnt[0][1] = -0.25f;  _bPnt[0][2] = 0.25f;  _bPnt[0][3] = 1.0f; // right
	_bPnt[1][0] = 0.5f;    _bPnt[1][1] = 0.25f;   _bPnt[1][2] = -0.25f; _bPnt[1][3] = 1.0f; // right
	_bPnt[2][0] = -0.25f;  _bPnt[2][1] = 0.5f;    _bPnt[2][2] = 0.25f;  _bPnt[2][4] = 1.0f; // top
	_bPnt[3][0] = 0.25f;   _bPnt[3][1] = 0.5f;    _bPnt[3][2] = -0.25f; _bPnt[3][4] = 1.0f; // top
	_bPnt[4][0] = -0.25f;  _bPnt[4][1] = -0.25f;  _bPnt[4][2] = 0.5f;   _bPnt[4][5] = 1.0f; // front
	_bPnt[5][0] = 0.25f;   _bPnt[5][1] = 0.25f;   _bPnt[5][2] = 0.5f;   _bPnt[5][5] = 1.0f; // front
	// diagonal border points
	_bPnt[6][0] = 0.5f;    _bPnt[6][1] = -0.5f;   _bPnt[6][2] = -0.5f;  _bPnt[6][3] = 1.0f; _bPnt[6][4] = -1.0f; _bPnt[6][5] = -1.0f;
	_bPnt[7][0] = 0.5f;    _bPnt[7][1] = 0.5f;    _bPnt[7][2] = -0.5f;  _bPnt[7][3] = 1.0f; _bPnt[7][4] = 1.0f;  _bPnt[7][5] = -1.0f;
	_bPnt[8][0] = 0.5f;    _bPnt[8][1] = -0.5f;   _bPnt[8][2] = 0.5f;   _bPnt[8][3] = 1.0f; _bPnt[8][4] = -1.0f; _bPnt[8][5] = 1.0f;
	_bPnt[9][0] = 0.5f;    _bPnt[9][1] = 0.5f;    _bPnt[9][2] = 0.5f;   _bPnt[9][3] = 1.0f; _bPnt[9][4] = 1.0f;  _bPnt[9][5] = 1.0f;
	
	_globalxyz[0] = 0;
	_globalxyz[1] = 0;
	_globalxyz[2] = 0;
	
	// Set up first path section
	_path[0][0] = 0.0f;
	_path[0][1] = 0.0f;
	_path[0][2] = 0.0f;
	_path[0][3] = 0.0f;
	_path[0][4] = 0.0f;
	_path[0][5] = 0.0f;
	j = rsRandi(12);
	k = j % 6;
	for(i=0; i<6; i++)
		_path[1][i] = _bPnt[k][i];
	if(j > 5)
	{  // If we want to head in a negative direction
		i = k / 2;  // then we need to flip along the appropriate axis
		_path[1][i] *= -1.0f;
		_path[1][i+3] *= -1.0f;
	}
	_lastBorder = k;
	_segments = 1;
	
	struct timeval tTime;
	gettimeofday(&tTime, NULL);
	
	_lastRefresh=(tTime.tv_sec*1000+tTime.tv_usec/1000);
}

void scene::resize(int inWidth,int inHeight)
{
	if (_camera!=NULL)
	{
		delete _camera;
		_camera=NULL;
	}
	
	if(dWidescreen)
		glViewport(0, inHeight/2-inWidth/4, inWidth, inWidth/2);
	else
		glViewport(0,0, inWidth,inHeight);
	
	if(torusTexture != LATTICE_TEXTURE_CRYSTAL && torusTexture != LATTICE_TEXTURE_GHOSTLY)  // No z-buffering for crystal or ghostly
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float mat[16] = {(float) cos(float(dFov) * 0.5f * D2R) / (float) sin(float(dFov) * 0.5f * D2R), 0.0f, 0.0f, 0.0f,
		0.0f, 0.0, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f - 0.02f / float(dDepth), -1.0f,
		0.0f, 0.0f, -(0.02f + 0.0002f / float(dDepth)), 0.0f};
	if(dWidescreen)
		//gluPerspective(asinf(sinf(float(dFov)*0.5f*D2R)*0.5f)*2.0f*R2D, 2.0f, 0.01f, float(dDepth));
		//gluPerspective(float(dFov), 2.0f, 0.01f, float(dDepth));
		mat[5] = mat[0] * 2.0f;
	else
		//gluPerspective(asinf(sinf(float(dFov)*0.5f*D2R)*0.75f)*2.0f*R2D, 1.333f, 0.01f, float(dDepth));
		//gluPerspective(float(dFov), 1.333f, 0.01f, float(dDepth));
		//mat[5] = mat[0] * 1.333f;
		mat[5] = mat[0] * (float(inWidth) / float(inHeight));
	glLoadMatrixf(mat);
	_camera = new camera;
	_camera->init(mat, float(dDepth));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
