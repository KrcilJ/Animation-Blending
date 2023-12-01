///////////////////////////////////////////////////
//
//	Hamish Carr
//	January, 2018
//
//	------------------------
//	HomogeneousFaceSurface.cpp
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


#include "HomogeneousFaceSurface.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// constructor will initialise to safe values
HomogeneousFaceSurface::HomogeneousFaceSurface()
	{ // HomogeneousFaceSurface::HomogeneousFaceSurface()
	// force the size to nil (should not be necessary, but . . .)
	vertices.resize(0);
	normals.resize(0);
	} // HomogeneousFaceSurface::HomogeneousFaceSurface()

// read routine returns true on success, failure otherwise
bool HomogeneousFaceSurface::ReadFileTriangleSoup(const char *fileName)
	{ // HomogeneousFaceSurface::ReadFileTriangleSoup()
	// open the input file
	std::ifstream inFile(fileName);
	if (inFile.bad()) 
		return false;
	
	// set the number of vertices and faces
	long nTriangles = 0, nVertices = 0;
	
	// read in the number of vertices
	inFile >> nTriangles;
	nVertices = nTriangles * 3;

	// now allocate space for them all
	vertices.resize(nVertices);
	
	// now loop to read the vertices in, and hope nothing goes wrong
	for (int vertex = 0; vertex < nVertices; vertex++)
		{ // for each vertex
		// read in the Cartesian coordinates
		inFile >> vertices[vertex].x >> vertices[vertex].y >> vertices[vertex].z;
		// set the homogeneous coordinate to 1 directly
		vertices[vertex].w = 1.0;
		} // for each vertex

	// call the routine to compute normals
	ComputeUnitNormalVectors();

	return true;
	} // HomogeneousFaceSurface::ReadFileTriangleSoup()

// routine to compute unit normal vectors
void HomogeneousFaceSurface::ComputeUnitNormalVectors()
	{ // ComputeUnitNormalVectors()
	// assume that the triangle vertices are set correctly, and allocate one third of that for normals
	normals.resize(vertices.size() / 3);
	
	// loop through the triangles, computing normal vectors
	for (int triangle = 0; triangle < (int) normals.size(); triangle++)
		{ // per triangle
		// retrieve the three vertices in Cartesian form
		Cartesian3 vertexP = vertices[3 * triangle	].Point();
		Cartesian3 vertexQ = vertices[3 * triangle + 1	].Point();
		Cartesian3 vertexR = vertices[3 * triangle + 2	].Point();
		// compute two edge vectors
		Cartesian3 vectorU = vertexQ - vertexP;
		Cartesian3 vectorV = vertexR - vertexP;
		// compute a normal with the cross-product
		Cartesian3 normal = vectorU.cross(vectorV).unit();
		// and store it as a homogeneous vector
		normals[triangle] = Homogeneous4(normal.x, normal.y, normal.z, 0.0);			
		} // per triangle
	} // ComputeUnitNormalVectors()

// routine to render
void HomogeneousFaceSurface::Render(Matrix4 &viewMatrix)
	{ // HomogeneousFaceSurface::Render()
	// walk through the faces rendering each one
	glBegin(GL_TRIANGLES);
	
	// we loop through all of the triangles
	for (int triangle = 0; triangle < (int) normals.size(); triangle++)
		{ // per triangle
		// retrieve the vertices and the normal
		Homogeneous4 vertexP 	= viewMatrix * vertices[3 * triangle		];
		Homogeneous4 vertexQ 	= viewMatrix * vertices[3 * triangle + 1	];
		Homogeneous4 vertexR 	= viewMatrix * vertices[3 * triangle + 2	];

		// normal vector is tricky because we need NOT to apply the translation component
		// so we create a temporary matrix and zero its translation elements
		Matrix4 normalMatrix = viewMatrix;
		normalMatrix[0][3] = normalMatrix[1][3] = normalMatrix[2][3] = 0.0;
		
		// now we multiply to get the correct normal		
		Homogeneous4 normal 	= normalMatrix * normals[triangle];
		
		// this works because C++ guarantees that the POD data is in exactly
		// the order stated in the class with no padding.
		glNormal3fv(&normal.x);
		glVertex4fv(&vertexP.x);
		glVertex4fv(&vertexQ.x);
		glVertex4fv(&vertexR.x);
		} // per triangle
	
	glEnd();
	} // HomogeneousFaceSurface::Render()

// routine to dump out as triangle soup
void HomogeneousFaceSurface::WriteTriangleSoup()
	{ // HomogeneousFaceSurface::WriteTriangleSoup()
	std::cout << normals.size() << std::endl;
	for (int triangle = 0; triangle < (int) normals.size(); triangle++)
		std::cout << std::fixed << vertices[3 * triangle] << "\t\t" << vertices[3 * triangle + 1] << "\t\t" << vertices[3 * triangle +2] << std::endl;
	} // HomogeneousFaceSurface::WriteTriangleSoup()

