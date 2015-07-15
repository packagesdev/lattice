#ifndef __LATTICE__
#define __LATTICE__

#include "rsMath.h"

#include "camera.h"

#define PI 3.14159265359f
#define PIx2 6.28318530718f
#define D2R 0.0174532925f
#define R2D 57.2957795131f
#define NUMOBJECTS 10
#define LATSIZE 10

#define LATTICE_TEXTURE_NONE		0
#define LATTICE_TEXTURE_INDUSTRIAL	1
#define LATTICE_TEXTURE_CRYSTAL		2
#define LATTICE_TEXTURE_CHROME		3
#define LATTICE_TEXTURE_BRASS		4
#define LATTICE_TEXTURE_SHINY		5
#define LATTICE_TEXTURE_GHOSTLY		6
#define LATTICE_TEXTURE_CIRCUITS	7
#define LATTICE_TEXTURE_DOUGHNUTS	8
#define LATTICE_TEXTURE_RANDOM		9


class scene
{
public:
	
	int dLongitude;
	int dLatitude;
	int torusThickness;
	int torusTexture;
	
	int dDepth;
	int dFov;
	int dPathrand;
	int dSpeed;
	
	
	bool dSmooth;
	bool dFog;
	bool dWidescreen;
	
	int density;
	
	scene();
	virtual ~scene();
	
	void create();
	void resize(int inWidth,int inHeight);
	void draw(void);

private:
	
	unsigned long long _lastRefresh;
	
	int _segments;
	int _seg;  // Section of path
	float _where;
	
	rsVec _oldxyz;
	rsVec _oldDir;
	rsVec _oldAngvel;
	
	rsQuat _quat;
	
	int _flymode;
	float _flymodeChange;
	float _rollVel;
	float _rollAcc;
	float _rollChange;
	
	
	
	
	camera* _camera;
	
	int _lattice[LATSIZE][LATSIZE][LATSIZE];
	int _globalxyz[3];
	int _lastBorder;
	float ** _bPnt;  // Border points and direction vectors where camera can cross from cube to cube
	float ** _path;
	
	void initTextures(void);
	
	void makeLatticeObjects(void);
	void reconfigure(void);
	
	static void makeTorus(int smooth, int longitude, int latitude, float centerradius, float thickradius);
};

#endif