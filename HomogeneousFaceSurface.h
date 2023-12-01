///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	HomogeneousFaceSurface.h
//	------------------------
//	
//	This is a modified version of the Geometric Processing
//	GeometricSurfaceFaceDS class that uses Homogeneous4 
// 	instead of Cartesian coordinates and that precomputes
//	normal vectors for all of the triangles
//	This version DOES NOT compute bounding spheres or midpoints
//	ALL transformations are up to the user.
//	
///////////////////////////////////////////////////

#ifndef _HOMOGENEOUS_FACE_SURFACE_H
#define _HOMOGENEOUS_FACE_SURFACE_H

#include <vector>

#include "Homogeneous4.h"
#include "Matrix4.h"

class HomogeneousFaceSurface
	{ // class HomogeneousFaceSurface
	public:
	// vector to store vertex and triangle information
	// each three vertices will form a single triangle
	std::vector<Homogeneous4> vertices;

	// vector to hold corresponding normal vectors
	std::vector<Homogeneous4> normals;

	// constructor will initialise to safe values
	HomogeneousFaceSurface();
	
	// read routine returns true on success, failure otherwise
	bool ReadFileTriangleSoup(const char *fileName);
	
	// routine to compute unit normal vectors
	void ComputeUnitNormalVectors();
	
	// routine to render
	void Render(Matrix4 &viewMatrix);
	
	// routine to dump out as triangle soup
	void WriteTriangleSoup();	

	// render grid for debuging
	void RenderGrid(Matrix4& viewMatrix);

	// render pyramid for debuging
	void RenderPyramid(Matrix4& viewMatrix);
	
	}; // class HomogeneousFaceSurface

#endif
