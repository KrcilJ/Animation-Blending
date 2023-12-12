///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	SceneModel.h
//	------------------------
//	
//	The model of the scene
//	
///////////////////////////////////////////////////

#ifndef __SCENE_MODEL_H
#define __SCENE_MODEL_H

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
#include "Terrain.h"
#include "BVHData.h"
#include "Matrix4.h"

class SceneModel										
	{ // class SceneModel
	public:	
	// a terrain model 
	Terrain groundModel;

	// animation cycles (which implicitly have geometric data for a character)
	BVHData restPose;
	BVHData runCycle;
	BVHData veerLeftCycle;
	BVHData veerRightCycle;
    BVHData walking;
    BVHData blendedAnimation;
    // location & orientation of character
    Cartesian3 characterLocation = Cartesian3(0, 0, 0);
    Matrix4 characterRotation = Matrix4::Identity();
    BVHData currCycle;
    std::string runDir = "forward";
    Cartesian3 characterSpeed = Cartesian3(0, -0.5f, 0);
    // a matrix that specifies the mapping from world coordinates to those assumed
    // by OpenGL
    Matrix4 world2OpenGLMatrix;
    std::vector<std::vector<Cartesian3>> blendedBoneRotations;
    // matrix for user camera
    Matrix4 viewMatrix;
    Matrix4 CameraTranslateMatrix;
    Matrix4 CameraRotationMatrix;
    int startFrame = 24;          // frame to start rotating
    int endFrame = 33;            // frame to end rotation
    float totalRotation = -90.0f; // Total rotation expected to be done by the character
    // the frame number for use in animating
    unsigned long frameNumber;
    // the frame number for use in animating
    unsigned long blendingStartFrame = -1;
    unsigned long blendingEndFrame = 0;
    // constructor
    SceneModel();

    // routine that updates the scene for the next frame
    void Update();

    // routine to tell the scene to render itself
    void Render();

    // camera control events: WASD for motion
    void EventCameraForward();
	void EventCameraLeft();
	void EventCameraRight();
	void EventCameraBackward();
	// camera control events: RF for vertical motion
	void EventCameraUp();
	void EventCameraDown();
	// camera rotation events: QE for left and right
	void EventCameraTurnLeft();
	void EventCameraTurnRight();
	
	// character motion events: arrow keys for forward, backward, veer left & right
	void EventCharacterTurnLeft();
	void EventCharacterTurnRight();
	void EventCharacterForward();
	void EventCharacterBackward();

	// reset character to original position: p
	void EventCharacterReset();
    float calcRotation(int animationFrame);
    // needed for now for Xiaoyuan's code
    void EventSwitchMode();
    void blendBonerotations(std::vector<std::vector<Cartesian3>> &boneRotations, int animationFrame);
    void blendAnimation(std::string newPose, BVHData &newBVH);
    }; // class SceneModel

#endif
	
