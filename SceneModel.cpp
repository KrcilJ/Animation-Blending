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
const char *motionBvhWalk = "./models/walking.bvh";
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
    walking.ReadFileBVH(motionBvhWalk);
    currCycle = restPose;
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

    if (runDir == "right") {
        totalRotation = 90.0f;
    } else if (runDir == "left") {
        totalRotation = -90.0f;
    } else {
        totalRotation = 0.0f;
    }

    //runCycle.Render(moveMat, 0.1f, (frameNumber) % runCycle.frame_count);
    if (frameNumber >= blendingStartFrame && frameNumber < blendingEndFrame) {
        Matrix4 blendedMoveMat = viewMatrix * Matrix4::Translate(characterLocation)
                                 * characterRotation;
        //move the character according to the ground height
        Cartesian3 pos = (Matrix4::Translate(characterLocation) * characterRotation).column(3).Vector();
        blendedMoveMat = blendedMoveMat * Matrix4::Translate(Cartesian3(0,0,groundModel.getHeight(pos.x, pos.y)));
        blendedAnimation.Render(blendedMoveMat, 0.1f, frameNumber - blendingStartFrame - 1);

    } else {
        int animationFrame = (frameNumber - blendingEndFrame) % currCycle.frame_count;
        calcRotation(animationFrame);
        characterLocation = characterLocation + characterRotation * characterSpeed;
        Matrix4 moveMat = viewMatrix * Matrix4::Translate(characterLocation) * characterRotation;

        //std::cout << "normal frame" << animationFrame << std::endl;
        Cartesian3 pos = (Matrix4::Translate(characterLocation) * characterRotation).column(3).Vector();
        //move the character according to the ground height
        moveMat = moveMat * Matrix4::Translate(Cartesian3(0,0,groundModel.getHeight(pos.x, pos.y)));
        currCycle.Render(moveMat, 0.1f, animationFrame);
    }

    //walking.Render(viewMatrix, 0.1f, (frameNumber) % walking.frame_count);
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
    if(runDir != "left"){
        std::vector<std::vector<Cartesian3>> currRotations = currCycle.boneRotations;
        int animationFrame = frameNumber % currCycle.frame_count;
        blendedAnimation = currCycle;
        currCycle = veerLeftCycle;
        blendBonerotations(currRotations, animationFrame);

        runDir = "left";
        blendingStartFrame = frameNumber;
        blendingEndFrame = frameNumber + 13;
    }
    } // EventCharacterTurnLeft()

    void SceneModel::EventCharacterTurnRight()
    { // EventCharacterTurnRight()
    if(runDir != "right"){
        std::vector<std::vector<Cartesian3>> currRotations = currCycle.boneRotations;
        int animationFrame = frameNumber % currCycle.frame_count;
        blendedAnimation = currCycle;
        currCycle = veerRightCycle;
        blendBonerotations(currRotations, animationFrame);

        runDir = "right";
        blendingStartFrame = frameNumber;
        blendingEndFrame = frameNumber + 13;
    }


    } // EventCharacterTurnRight()

    void SceneModel::EventCharacterForward()
    { // EventCharacterForward()
        if(runDir != "forward"){
            std::vector<std::vector<Cartesian3>> currRotations = currCycle.boneRotations;
            int animationFrame = frameNumber % currCycle.frame_count;
            blendedAnimation = currCycle;
             currCycle = runCycle;
            blendBonerotations(currRotations, animationFrame);
            this->characterSpeed = Cartesian3(0, -0.2f, 0);
            runDir = "forward";
            blendingStartFrame = frameNumber;
            blendingEndFrame = frameNumber + 13;
        }

    } // EventCharacterForward()

    void SceneModel::EventCharacterBackward()
    { // EventCharacterBackward()
        if(runDir != "rest"){
            std::vector<std::vector<Cartesian3>> currRotations = currCycle.boneRotations;
            int animationFrame = frameNumber % currCycle.frame_count;
            blendedAnimation = currCycle;
             currCycle = restPose;
            blendBonerotations(currRotations, animationFrame);
            this->characterSpeed = Cartesian3(0, 0, 0);
            runDir = "rest";
            blendingStartFrame = frameNumber;
            blendingEndFrame = frameNumber + 13;
        }

    } // EventCharacterBackward()

    // reset character to original position: p
    void SceneModel::EventCharacterReset()
    { // EventCharacterReset()
    this->characterLocation = Cartesian3(0, 0, 0);
    this->characterRotation = Matrix4::Identity();
    currCycle = restPose;
    runDir = "rest";
    this->characterSpeed = Cartesian3(0, 0, 0);
    } // EventCharacterReset()

    float SceneModel::calcRotation(int animationFrame)
    {
    if (animationFrame >= startFrame && animationFrame < endFrame) {
        float relativeRotation = totalRotation / (endFrame - startFrame);
        characterRotation = Matrix4::RotateZ(relativeRotation) * characterRotation;
        return relativeRotation;
    }
    }
    void SceneModel::blendBonerotations(std::vector<std::vector<Cartesian3>> &boneRotations,
                                        int animationFrame)
    {
    int blendSteps = 12;
    float t = 1.0f;
    float tStep = t / (blendSteps - 1.0f);
    blendedAnimation.boneRotations.resize(blendSteps);
    for(int i =0; i < blendSteps; ++i){
        blendedAnimation.boneRotations[i].resize(65);
    }
    for (int i = 0; i < blendSteps; ++i) {
        for (int joint = 0; joint < boneRotations[0].size(); ++joint) {
             // std::cout << boneRotations[animationFrame][joint] << " " << joint<< std::endl;
            blendedAnimation.boneRotations[i][joint] = std::max(t, 0.0f)
                                                           * boneRotations[animationFrame][joint]
                                                       + ((1.0f - t) <= 1.0f ? (1.0f - t) : 1.0f)
                                                             * currCycle.boneRotations[0][joint];

        }
        t -= tStep;
    }
    }
