///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	SceneModel.cpp
//	------------------------
//	
//	The model of the scene
//
//	
///////////////////////////////////////////////////

#include "SceneModel.h"
#include <math.h>

// three local variables with the hardcoded file names
const char* groundModelName		= "./models/randomland.dem";
const char* characterModelName	= "./models/human_lowpoly_100.obj";
const char* motionBvhStand		= "./models/stand.bvh";
const char* motionBvhRun		= "./models/fast_run.bvh";
const char* motionBvhveerLeft	= "./models/veer_left.bvh";
const char* motionBvhveerRight	= "./models/veer_right.bvh";
const float cameraSpeed = 0.5;

const Homogeneous4 sunDirection(0.5, -0.5, 0.3, 1.0);
const GLfloat groundColour[4] = { 0.2, 0.5, 0.2, 1.0 };
const GLfloat boneColour[4] = { 0.7, 0.7, 0.4, 1.0 };
const GLfloat sunAmbient[4] = {0.1, 0.1, 0.1, 1.0 };
const GLfloat sunDiffuse[4] = {0.7, 0.7, 0.7, 1.0 };
const GLfloat blackColour[4] = {0.0, 0.0, 0.0, 1.0};

// constructor
SceneModel::SceneModel()
	{ // constructor
	// load the object models from files
	groundModel.ReadFileTerrainData(groundModelName, 3);

	// load the animation data from files
	restPose.ReadFileBVH(motionBvhStand);
	runCycle.ReadFileBVH(motionBvhRun);
	veerLeftCycle.ReadFileBVH(motionBvhveerLeft);
	veerRightCycle.ReadFileBVH(motionBvhveerRight);

    // set the world to opengl matrix
    world2OpenGLMatrix = Matrix4::RotateX(90.0);
    CameraTranslateMatrix = Matrix4::Translate(Cartesian3(-5, 15, -15.5));
    CameraRotationMatrix = Matrix4::RotateX(-30.0) * Matrix4::RotateZ(15.0);

    // initialize the character's position and rotation
    EventCharacterReset();

    // and set the frame number to 0
    frameNumber = 0;

    } // constructor

    // routine that updates the scene for the next frame
    void SceneModel::Update()
    { // Update()
    // increment the frame counter
    frameNumber++;

    } // Update()

    // routine to tell the scene to render itself
    void SceneModel::Render()
    { // Render()
    // enable Z-buffering
    glEnable(GL_DEPTH_TEST);

    // set lighting parameters
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, blackColour);
    glLightfv(GL_LIGHT0, GL_EMISSION, blackColour);

    // background is sky-blue
    glClearColor(0.7, 0.7, 1.0, 1.0);

    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // compute the view matrix by combining camera translation, rotation & world2OpenGL
    viewMatrix = world2OpenGLMatrix * CameraRotationMatrix * CameraTranslateMatrix;

    // compute the light position
    Homogeneous4 lightDirection = world2OpenGLMatrix * CameraRotationMatrix * sunDirection;

    // turn it into Cartesian and normalise
    Cartesian3 lightVector = lightDirection.Vector().unit();

    // and set the w to zero to force infinite distance
    lightDirection.w = 0.0;

    // pass it to OpenGL
    glLightfv(GL_LIGHT0, GL_POSITION, &(lightVector.x));

    // and set a material colour for the ground
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundColour);
    glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
    glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

    // render the terrain
    groundModel.Render(viewMatrix);

    // now set the colour to draw the bones
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, boneColour);

    restPose.Render(viewMatrix, 0.1f, 0);
    //runCycle.Render(viewMatrix, 0.1f, (frameNumber) % runCycle.frame_count);
    } // Render()

    // camera control events: WASD for motion
    void SceneModel::EventCameraForward()
    { // EventCameraForward()
    // update the camera matrix
    CameraTranslateMatrix = CameraTranslateMatrix * CameraRotationMatrix.transpose()
                            * Matrix4::Translate(Cartesian3(0.0f, -cameraSpeed, 0.0f))
                            * CameraRotationMatrix;
    } // EventCameraForward()

void SceneModel::EventCameraBackward()
	{ // EventCameraBackward()
	// update the camera matrix
	CameraTranslateMatrix = CameraTranslateMatrix * CameraRotationMatrix.transpose() * Matrix4::Translate(Cartesian3(0.0f, cameraSpeed, 0.0f)) * CameraRotationMatrix;
	} // EventCameraBackward()

void SceneModel::EventCameraLeft()
	{ // EventCameraLeft()
	// update the camera matrix
	CameraTranslateMatrix = CameraTranslateMatrix * CameraRotationMatrix.transpose() * Matrix4::Translate(Cartesian3(cameraSpeed, 0.0f, 0.0f)) * CameraRotationMatrix;
	} // EventCameraLeft()
	
void SceneModel::EventCameraRight()
	{ // EventCameraRight()
	// update the camera matrix
	CameraTranslateMatrix = CameraTranslateMatrix * CameraRotationMatrix.transpose() * Matrix4::Translate(Cartesian3(-cameraSpeed, 0.0f, 0.0f)) * CameraRotationMatrix;
	} // EventCameraRight()

// camera control events: RF for vertical motion
void SceneModel::EventCameraUp()
	{ // EventCameraUp()
	// update the camera matrix
	CameraTranslateMatrix = CameraTranslateMatrix * CameraRotationMatrix.transpose() * Matrix4::Translate(Cartesian3(0.0f, 0.0f, -cameraSpeed)) * CameraRotationMatrix;
	} // EventCameraUp()
	
void SceneModel::EventCameraDown()
	{ // EventCameraDown()
	// update the camera matrix
	CameraTranslateMatrix = CameraTranslateMatrix * CameraRotationMatrix.transpose() * Matrix4::Translate(Cartesian3(0.0f, 0.0f, cameraSpeed)) * CameraRotationMatrix;
	} // EventCameraDown()

// camera rotation events: QE for left and right
void SceneModel::EventCameraTurnLeft()
	{ // EventCameraTurnLeft()
	CameraRotationMatrix = CameraRotationMatrix * Matrix4::RotateZ(2.0f);
	} // EventCameraTurnLeft()

void SceneModel::EventCameraTurnRight()
	{ // EventCameraTurnRight()
	CameraRotationMatrix = CameraRotationMatrix * Matrix4::RotateZ(-2.0f);
	} // EventCameraTurnRight()
	
// character motion events: arrow keys for forward, backward, veer left & right
void SceneModel::EventCharacterTurnLeft()
	{ // EventCharacterTurnLeft()
	} // EventCharacterTurnLeft()
	
void SceneModel::EventCharacterTurnRight()
	{ // EventCharacterTurnRight()
	} // EventCharacterTurnRight()
	
void SceneModel::EventCharacterForward()
	{ // EventCharacterForward()
	} // EventCharacterForward()
	
void SceneModel::EventCharacterBackward()
	{ // EventCharacterBackward()
	} // EventCharacterBackward()

// reset character to original position: p
void SceneModel::EventCharacterReset()
	{ // EventCharacterReset()
	this->characterLocation = Cartesian3(0, 0, 0);
	this->characterRotation = Matrix4::Identity();
	} // EventCharacterReset()
