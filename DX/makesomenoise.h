////////////////////////////////////////////////////////////////////////////////
// Filename: makesomenoise.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MAKESOMENOISE_H_
#define _MAKESOMENOISE_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: MakeSomeNoise
////////////////////////////////////////////////////////////////////////////////
class MakeSomeNoise
{
public:
	MakeSomeNoise();

	double Perlin(double x, double y, double z = 0);
	double Simplex(double x, double y, double z = 0);

private:
	int fastfloor(double x);
	double dot(const int g[], double x, double y, double z);
	double mix(double a, double b, double t);
	double fade(double t);

	int perm[512];

	const static int grad3[12][3];

	const static int p[];
};

#endif