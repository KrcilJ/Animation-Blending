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
    //set the rotation based on the direction the character is running in
    if (runDir == "right") {
        totalRotation = 90.0f;
    } else if (runDir == "left") {
        totalRotation = -90.0f;
    } else {
        totalRotation = 0.0f;
    }

    //run this when we are blending
    if (frameNumber >= blendingStartFrame && frameNumber < blendingEndFrame) {
        //calculate the tranformation by the character location and rotation
        Matrix4 blendedMoveMat = viewMatrix * Matrix4::Translate(characterLocation)
                                 * characterRotation;
        //move the character according to the ground height
        Cartesian3 pos = (Matrix4::Translate(characterLocation) * characterRotation).column(3).Vector();
        blendedMoveMat = blendedMoveMat * Matrix4::Translate(Cartesian3(0,0,groundModel.getHeight(pos.x, pos.y)));
        blendedAnimation.Render(blendedMoveMat, 0.1f, frameNumber - blendingStartFrame - 1);
    }
    //run this if we are not blending
    else {
        int animationFrame = (frameNumber - blendingEndFrame) % currCycle.frame_count;
        calcRotation(animationFrame);
        characterLocation = characterLocation + characterRotation * characterSpeed;
        Matrix4 moveMat = viewMatrix * Matrix4::Translate(characterLocation) * characterRotation;


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
         blendAnimation("left", veerLeftCycle);
    }
    } // EventCharacterTurnLeft()

    void SceneModel::EventCharacterTurnRight()
    { // EventCharacterTurnRight()
    if(runDir != "right"){
       blendAnimation("right", veerRightCycle);
    }
    } // EventCharacterTurnRight()

    void SceneModel::EventCharacterForward()
    { // EventCharacterForward()
        if(runDir != "forward"){
           blendAnimation("forward", runCycle);
           this->characterSpeed = Cartesian3(0, -0.4f, 0);
        }

    } // EventCharacterForward()

    void SceneModel::EventCharacterBackward()
    { // EventCharacterBackward()
        if(runDir != "rest"){
         blendAnimation("rest", restPose);
         this->characterSpeed = Cartesian3(0, 0, 0);
        }

    } // EventCharacterBackward()

    // reset character to original position: p
    void SceneModel::EventCharacterReset()
    { // EventCharacterReset()
    this->characterLocation = Cartesian3(0, 0, 0);
    this->characterSpeed = Cartesian3(0, 0, 0);
    this->characterRotation = Matrix4::Identity();
    currCycle = restPose;
    runDir = "rest";
    } // EventCharacterReset()

    float SceneModel::calcRotation(int animationFrame)
    {
        //calculate the angle in deg of how much the character should rotate each frame (based on the start and end frame which can be eddited in the header)
    if (animationFrame >= startFrame && animationFrame < endFrame) {
        float relativeRotation = totalRotation / (endFrame - startFrame);
        //update the character rotation
        characterRotation = Matrix4::RotateZ(relativeRotation) * characterRotation;
        return relativeRotation;
    }
    }
    void SceneModel::blendBonerotations(std::vector<std::vector<Cartesian3>> &boneRotations,
                                        int animationFrame)
    {
    //number of steps to blend the animation in (12 frames is roughly 0.5s as our renderer is running at 24 fps)
    int blendSteps = 12;
    float t = 1.0f;
    float tStep = t / (blendSteps - 1.0f);
    //resize the bonerotaions of the blended animation
    blendedAnimation.boneRotations.resize(blendSteps);
    for(int i =0; i < blendSteps; ++i){
        blendedAnimation.boneRotations[i].resize(65);
    }
    for (int i = 0; i < blendSteps; ++i) {
        for (int joint = 0; joint < boneRotations[0].size(); ++joint) {
            //do linear interpolation between the two angles and save them in the vector
            blendedAnimation.boneRotations[i][joint] = std::max(t, 0.0f)
                                                           * boneRotations[animationFrame][joint]
                                                       + ((1.0f - t) <= 1.0f ? (1.0f - t) : 1.0f)
                                                             * currCycle.boneRotations[0][joint];
        }
        t -= tStep;
    }
    }

    void SceneModel::blendAnimation(std::string newPose, BVHData &nextBVH){
        //save the boneRotations of the current animation cycle
        std::vector<std::vector<Cartesian3>> currRotations = currCycle.boneRotations;
        //check in which frame the animation is
        int animationFrame = frameNumber % currCycle.frame_count;
        // set the new blended cycle to be the current cycle
        blendedAnimation = currCycle;
        //set the new animation cycle to be the current one
        currCycle = nextBVH;
        //interpolate rotations
        blendBonerotations(currRotations, animationFrame);
        //set new run direction
        runDir = newPose;
        //set the frames to blend the animation in
        blendingStartFrame = frameNumber;
        blendingEndFrame = frameNumber + 13;
    }
