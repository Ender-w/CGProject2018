#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "external_files/irrklang/irrKlang.h"
using namespace irrklang;

#define STB_IMAGE_IMPLEMENTATION
#include "external_files/stb_image.h"

#include "external_files/tiny_obj_loader.h"
using namespace std;

#include "external_files/InputState.h"
#include "Mesh.h"
#include "Floor.h"
#include "external_files/Shader.hpp"
#include "Lighting.h"
#include "SegmentRandomiser.h"
#include "Skybox.h"
#include "ProceduralGenerator.h"

/**
	 Computer Graphics Assignment 3 part 2
	 by a1688639 & a1672504
*/

Mesh *object1;
Mesh *tree1;
Mesh *tree2;

Skybox *skybox1;

Lighting *lightObject;
Floor *floor1;

ProceduralGenerator *generator;

int winX = 500;
int winY = 500;

glm::vec3 cameraStartPos(0.0f, 0.0f, 0.0f);//changing this may require changing the camera init center point

// Data structure storing mouse input info
InputState Input;
int shaderMode = 1;
int lightingMode = 0;
int lightToggle = 1;
int textureToggle = 1;
bool wireframe = false;

glm::mat4 viewMtx;

//---Game Variables---
glm::vec3 playerPosition(0.0f, 0.0f, -10.0f);
int playerX = 0;//int values for discrete calculations
int playerY = 0;
vector< vector< vector<int> > > mapVector; //Good lord that looks messy - essentially a vector of 3x3 matrices for the map
int mapLength = 10;//Number of segments in rendered distance
float playerSpeed = 6.5f;
double posChangeSpeed = 3.5f;//transition between positions speed
float mapObjectLength = 1.0f;//How big each object is - used in collision detection
float mapSegmentGap = 10.0f;//How far between each segment
double stanceChangeTime = 0.6;//How long the player jumps/crouches for in s
double timeSinceStanceChange = stanceChangeTime;
bool playing = false;//Stop game when you lose
bool gameover = false;
bool segmentFlag = false;
int segmentsPassed = 0;
clock_t deltaTimeStart;
ISoundEngine *SoundEngine = createIrrKlangDevice();


unsigned int programID;
unsigned int lightProgramID;
unsigned int skyboxProgramID;

void addToMap();
bool checkCollision(vector< vector<int> > playerColl, vector< vector<int> > mapColl);
//
// Callbacks
//
void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch(key) 
            {
            case GLFW_KEY_ESCAPE: // escape key pressed
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case 'Q':
            	if(!gameover){
		        	playing = !playing;
		        	if(playing)
		        		cout << "Game Resumed" << endl;
		        	else
		        		cout << "Game Paused" << endl;
            	}
            	break;
            case 'D':
            	viewMtx = glm::rotate(viewMtx, 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
            	break;
            case 'A':
            	viewMtx = glm::rotate(viewMtx, -0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
            	break;
            case 'S':
            	viewMtx = glm::rotate(viewMtx, 0.2f, glm::vec3(1.0f, 0.0f, 0.0f));
            	break;
            case 'W':
            	viewMtx = glm::rotate(viewMtx, -0.2f, glm::vec3(1.0f, 0.0f, 0.0f));
            	break;
            case 'T':
            	if(textureToggle == 1){
            		textureToggle = 0;
            	}else{
            		textureToggle = 1;
            	}
            	break;
            case 'L':
            	if(lightToggle == 1){
            		lightToggle = 0;
            	}else{
            		lightToggle = 1;
            	}
            	break;
            case 'F':
            	wireframe = !wireframe;
            	if(wireframe){
            		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            	}else{
            		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            	}
          		break;
           	case GLFW_KEY_LEFT:
           		if(playerX > -1){
           			playerX--;
           		}
           		break;
           	case GLFW_KEY_RIGHT:
           		if(playerX < 1){
           			playerX++;
           		}
           		break;
           	case GLFW_KEY_UP:
				if(playerY < 1){
           			playerY++;
           			timeSinceStanceChange = 0;
           		}
           		break;
           	case GLFW_KEY_DOWN:
           		if(playerY > -1){
           			playerY--;
           			timeSinceStanceChange = 0;
           		}
           		break;
            default:
                break;
            }
    }
}	

// Set the projection matrix. Takes into account window aspect ratio, so called
// when the window is resized.
void setProjection()
{
    glm::mat4 projection;
    
    projection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 0.001f, 40.0f );//40

	// Load it to the shader program
	glUseProgram(lightProgramID);
	int projHandle2 = glGetUniformLocation(lightProgramID, "projection");
	if (projHandle2 == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( projHandle2, 1, false, glm::value_ptr(projection) );
	
	glUseProgram(skyboxProgramID);
	int projHandle3 = glGetUniformLocation(skyboxProgramID, "projection");
	if (projHandle3 == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( projHandle3, 1, false, glm::value_ptr(projection) );
	
}    

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y ) 
{
    winX = x;
    winY = y;
    setProjection();
    glViewport( 0, 0, x, y );
}

void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    // Use a global data structure to store mouse info
    // We can then use it however we want
    Input.update((float)x, (float)y);
}    

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        Input.rMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        Input.rMousePressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        Input.lMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Input.lMousePressed = false;
    }                
}

void render() 
{

	glUseProgram(programID);
    // Update the camera, and draw the scene.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // First load the viewing matrix from the current camera - gone
    
	// Load it to the shader program
	int viewHandle = glGetUniformLocation(programID, "view");
	if (viewHandle == -1) {
		std::cout << "Uniform: view is not an active uniform label\n";
	}
	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMtx) );
	
	int lightToggleHandle = glGetUniformLocation(programID, "lightToggle");
	glUniform1i(lightToggleHandle, lightToggle);
	
	//render not the floor
	int floorHandle = glGetUniformLocation(programID, "floor");
	glUniform1i(floorHandle, 0);
	
	//---Game Logic---
	//Move the player	
	double deltaTime = (std::clock()-deltaTimeStart)/double(CLOCKS_PER_SEC)*1.0;
	deltaTimeStart = clock();
	if(playing){
	playerPosition.z += playerSpeed * deltaTime;//forward
	
	if((float)playerX < playerPosition.x){
		if(playerPosition.x - (posChangeSpeed * deltaTime) < (float)playerX){
			 playerPosition.x = (float)playerX;
		}else{
			playerPosition.x = playerPosition.x - (posChangeSpeed * deltaTime);
		}
	}else if((float)playerX > playerPosition.x){
		if(playerPosition.x + (posChangeSpeed * deltaTime) > (float)playerX){
			 playerPosition.x = (float)playerX;
		}else{
			playerPosition.x = playerPosition.x + (posChangeSpeed * deltaTime);
		}
	}
	if(timeSinceStanceChange < stanceChangeTime){
		timeSinceStanceChange += deltaTime;
	}else{
		playerY = 0;
	}
	if((float)playerY < playerPosition.y){
		if(playerPosition.y - (posChangeSpeed * deltaTime) < (float)playerY){
			 playerPosition.y = (float)playerY;
		}else{
			playerPosition.y = playerPosition.y - (posChangeSpeed * deltaTime);
		}
	}else if((float)playerY > playerPosition.y){
		if(playerPosition.y + (posChangeSpeed * deltaTime) > (float)playerY){
			 playerPosition.y = (float)playerY;
		}else{
			playerPosition.y = playerPosition.y + (posChangeSpeed * deltaTime);
		}
	}
	//Check Map collision
	vector< vector<int> > playerCollisionMatrix = {{0,0,0},{0,0,0},{0,0,0}};
	if(playerY == 1){
		if(playerX == 1){
			playerCollisionMatrix[0][2] = 1;
			playerCollisionMatrix[1][2] = 1;
		}else if(playerX == 0){
			playerCollisionMatrix[0][1] = 1;
			playerCollisionMatrix[1][1] = 1;
		}else{
			playerCollisionMatrix[0][0] = 1;
			playerCollisionMatrix[1][0] = 1;
		}
	}else if(playerY == 0){
		if(playerX == 1){
			playerCollisionMatrix[1][2] = 1;
			playerCollisionMatrix[2][2] = 1;
		}else if(playerX == 0){
			playerCollisionMatrix[1][1] = 1;
			playerCollisionMatrix[2][1] = 1;
		}else{
			playerCollisionMatrix[1][0] = 1;
			playerCollisionMatrix[2][0] = 1;
		}
	}else{
		if(playerX == 1){
			playerCollisionMatrix[2][2] = 1;
		}else if(playerX == 0){
			playerCollisionMatrix[2][1] = 1;
		}else{
			playerCollisionMatrix[2][0] = 1;
		}
	}
	
	bool flag = false;
	for(int i = 0; i < mapVector.size(); i++){//For each segment
		float segmentPos = i * mapSegmentGap;
		if(playerPosition.z > segmentPos - (mapObjectLength/2) && playerPosition.z < segmentPos + (mapObjectLength/2)){
			if(checkCollision(playerCollisionMatrix, mapVector[i])){
				cout << "You lose!" << endl;
				cout << "You passed " << segmentsPassed << " stages." << endl;
				SoundEngine->play2D("audio/explosion.wav");
				playing = false;
				gameover = true;
			}
			flag = true;
		}
	}
	if(flag){
		if(!segmentFlag){
			segmentFlag = true;
			segmentsPassed++;
		}
	}else{
		if(segmentFlag){
			segmentFlag = false;
		}
	}
	
	}//end playing
	int renderedSegments = 0;
	
    // Now draw the scene
    //Render skybox first
	skybox1->render(skyboxProgramID, viewMtx);
	generator->render(programID, playerPosition, tree1, tree2);//Render the terrain
    
    
    for(int i = 0; i < mapVector.size(); i++){//For each segment
    	float segmentPos = i * mapSegmentGap;
    	if(playerPosition.z < segmentPos){//Dont render things behind the player
    		renderedSegments++;
    		for(int x = 0; x < mapVector[i].size(); x++){
    			for(int z = 0 ; z < mapVector[i][x].size(); z++){
    				if(mapVector[i][x][z] == 1){
    					object1->render(programID, glm::vec3(z-1-playerPosition.x,(-(x-1))-playerPosition.y,-(segmentPos-playerPosition.z)), glm::vec3(0.5,0.5,0.5));
    				}
    			}
    		}
    	}
    }
    while(renderedSegments < mapLength){
    	renderedSegments++;
    	addToMap();
    }
    
    float player_pos[3] = {(float)playerPosition.x, (float)playerPosition.y, (float)playerPosition.z};
    lightObject -> render(lightProgramID, player_pos);
    floor1 -> render(lightProgramID, playerPosition, textureToggle);
}

bool checkCollision(vector< vector<int> > playerColl, vector< vector<int> > mapColl){
	for(int i = 0; i < playerColl.size(); i++){
		for(int x = 0; x < playerColl[i].size(); x++){
			if(playerColl[i][x] == 1){
				if(mapColl[i][x] == 1){
					return true;
				}
			}
		}
	}
	return false;
}

void setupMap(){
	srand(time(NULL));
	for(int i = 0; i < mapLength; i++){
		mapVector.push_back(chooseRandomSegment());
	}
}

void addToMap(){
	mapVector.push_back(chooseRandomSegment());
}

/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


int main (int argc, char **argv)
{	
	
	if(argc != 1){
		cerr << "Invalid number of arguments" << endl;
		exit(1);
	}
	
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        exit(1);
    }

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and OpenGL context
    window = glfwCreateWindow(winX, winY, "Running Game - Assignment 3 part 2", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	// Set up the shaders we are to use. 0 indicates error.
	lightProgramID = LoadShaders("light-shading.vert", "light-shading.frag");
	skyboxProgramID = LoadShaders("skybox.vert", "skybox.frag");
	//testProgram = LoadShaders("test.vert", "test.frag");
	if (lightProgramID == 0 || skyboxProgramID == 0) {
		exit(1);
    }
	programID = lightProgramID;
	
	//Info
	cout << "--------------------------------------" << endl;
	cout << "CG Project made by a1688639 & a1672504" << endl;
	cout << "Press 'Q' to pause/unpause. The game starts paused" << endl;
	cout << "Use 'wasd' keys to look around" << endl;
	cout << "use arrow keys to move" << endl;
	cout << "Press 'T' to toggle light effects on floor. Starts enabled" << endl;
	cout << "Press 'L' to toggle lighting on scene" << endl;
	cout << "Press 'F' to toggle wireframe mode" << endl;
	cout << "--------------------------------------" << endl;
	
	//Initialise Game logic/vars
	setupMap();
	
    // Set OpenGL state we need for this application.
    glClearColor(0.2F, 0.2F, 0.2F, 0.0F);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(programID);
    
    // Set up the scene and the camera
    setProjection();
    
    //Create Required Objects
    object1 = new Mesh("models/cube-tex.obj", programID);

    tree1 = new Mesh("models/PineTree03.obj", programID);
	tree2 = new Mesh("models/TreeStump03.obj", programID);

    skybox1 = new Skybox();
    
    floor1 = new Floor(programID);
        
    lightObject = new Lighting();
    lightObject->setupLightUniforms(lightProgramID);
    
    generator = new ProceduralGenerator(48);
    
    glUseProgram(programID);
   
   	//setup camera initialisation
   	glm::vec3 at(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    viewMtx = glm::lookAt(cameraStartPos, at, up);
	
    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//Setup texture uniforms
	int texture0Handle = glGetUniformLocation(lightProgramID, "texMap1");
	int texture1Handle = glGetUniformLocation(lightProgramID, "texMap2");
	int texture2Handle = glGetUniformLocation(lightProgramID, "texMap3");
	int texture3Handle = glGetUniformLocation(lightProgramID, "texMap4");
	int texture4Handle = glGetUniformLocation(lightProgramID, "diffuse_map");
	int texture5Handle = glGetUniformLocation(lightProgramID, "specular_map");
	int texture6Handle = glGetUniformLocation(lightProgramID, "normal_map");
	int texture7Handle = glGetUniformLocation(lightProgramID, "depth_map");
	
	if (texture0Handle == -1 ||
		texture1Handle == -1 ||
		texture2Handle == -1 ||
		texture3Handle == -1 ||
		texture4Handle == -1 ||
		texture5Handle == -1 ||
		texture6Handle == -1 ||
		texture7Handle == -1){
		fprintf(stderr, "Error: Floor Rendering can't find uniforms\n");
		exit(1);
	}
	glUniform1i(texture0Handle,0);
	glUniform1i(texture1Handle,1);
	glUniform1i(texture2Handle,2);
	glUniform1i(texture3Handle,3);
	glUniform1i(texture4Handle,4);
	glUniform1i(texture5Handle,5);
	glUniform1i(texture6Handle,6);
	glUniform1i(texture7Handle,7);
	

	deltaTimeStart = clock();

    while (!glfwWindowShouldClose(window))
    {
    	
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	
    // Clean up
    delete object1;
    delete tree1;
    delete tree2;
    delete skybox1;
    delete floor1;
    delete lightObject;
    delete generator;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);
    
    return 0;
}
