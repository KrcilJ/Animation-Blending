///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	Terrain.h
//	------------------------
//	
//	A subclass of HomogeneousFaceSurface for terrain
//	
///////////////////////////////////////////////////

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <vector>

#include "HomogeneousFaceSurface.h"

class Terrain : public HomogeneousFaceSurface
	{ // class Terrain
	public:
	// array to store the terrain data
	std::vector<std::vector<float>> heightValues;
	
	// keep track of the xy scale that we are told about
	float xyScale;

	// constructor will initialise to safe values
	Terrain();
	
	// read routine returns true on success, failure otherwise
	// xyScale gives the scale factor to use in the x-y directions
	bool ReadFileTerrainData(const char *fileName, float XYScale);
	
	// A function to find the height at a known (x,y) coordinate
	float getHeight(float x, float y);
	
	}; // class Terrain

#endif
