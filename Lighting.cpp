#include "Lighting.h"

#include <iostream>
#include <GL/glew.h>

/**
 * Lighting.cpp. Generates all lights and handles rendering aspects (such as movenment).
 * Author Mark - a1672504
 */

Lighting::Lighting(){
	lightCounter = 0;
}

void Lighting::render(unsigned int id, float* player_pos){
	glUseProgram(id);
	float lightposition[3];
	float offset;
	float position[3] = {player_pos[0], player_pos[1], player_pos[2] + 10};
	
	int store = (int)position[2];
	store = store + 30;
	store = store - store%60;
	int repeats1 = store / 60;
	
	int store2 = (int)position[2];
	store2 = store2 - store2%60;
	int repeats2 = store2 / 60;
	//---------------------------------------------------
	int lightpositionHandle = glGetUniformLocation(id, "light[2].poiLight.position");
	if ( lightpositionHandle == -1 ) {
	        fprintf(stderr, "Error: can't find light uniform variables in class\n");
			return;
	    }
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -5.0f;
	
	offset = 30;
	lightposition[0] = lightposition[0] - position[0];
	lightposition[1] = lightposition[1] - position[1];

	if(position[2] > ((float)repeats1)*60 - offset){
		lightposition[2] += ((float)repeats1)*-60;
	}
	lightposition[2] = lightposition[2] + position[2];

	glUniform3fv(lightpositionHandle, 1, lightposition);
	//---------------------------------------------------
	lightpositionHandle = glGetUniformLocation(id, "light[3].poiLight.position");
	if ( lightpositionHandle == -1 ) {
	        fprintf(stderr, "Error: can't find light uniform variables in class\n");
			return;
	    }
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -15.0f;
	
	offset = 20;
	lightposition[0] = lightposition[0] - position[0];
	lightposition[1] = lightposition[1] - position[1];

	if(position[2] > ((float)repeats1)*60 - offset){
		lightposition[2] += ((float)repeats1)*-60;
	}
	lightposition[2] = lightposition[2] + position[2];

	glUniform3fv(lightpositionHandle, 1, lightposition);
	//---------------------------------------------------
	lightpositionHandle = glGetUniformLocation(id, "light[4].poiLight.position");
	if ( lightpositionHandle == -1 ) {
	        fprintf(stderr, "Error: can't find light uniform variables in class\n");
			return;
	    }
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -25.0f;
	
	offset = 10;
	lightposition[0] = lightposition[0] - position[0];
	lightposition[1] = lightposition[1] - position[1];

	if(position[2] > ((float)repeats1)*60 - offset){
		lightposition[2] += ((float)repeats1)*-60;
	}
	lightposition[2] = lightposition[2] + position[2];

	glUniform3fv(lightpositionHandle, 1, lightposition);
	//---------------------------------------------------
	lightpositionHandle = glGetUniformLocation(id, "light[5].poiLight.position");
	if ( lightpositionHandle == -1 ) {
	        fprintf(stderr, "Error: can't find light uniform variables in class\n");
			return;
	    }
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -35.0f;
	
	offset = 0;
	lightposition[0] = lightposition[0] - position[0];
	lightposition[1] = lightposition[1] - position[1];

	if(position[2] > ((float)repeats2)*60 - offset){
		lightposition[2] += ((float)repeats2)*-60;
	}
	lightposition[2] = lightposition[2] + position[2];

	glUniform3fv(lightpositionHandle, 1, lightposition);
	//---------------------------------------------------
	lightpositionHandle = glGetUniformLocation(id, "light[6].poiLight.position");
	if ( lightpositionHandle == -1 ) {
	        fprintf(stderr, "Error: can't find light uniform variables in class\n");
			return;
	    }
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -45.0f;
	
	offset = -10;
	lightposition[0] = lightposition[0] - position[0];
	lightposition[1] = lightposition[1] - position[1];

	if(position[2] > ((float)repeats2)*60 - offset){
		lightposition[2] += ((float)repeats2)*-60;
	}
	lightposition[2] = lightposition[2] + position[2];

	glUniform3fv(lightpositionHandle, 1, lightposition);
	//---------------------------------------------------
	lightpositionHandle = glGetUniformLocation(id, "light[7].poiLight.position");
	if ( lightpositionHandle == -1 ) {
	        fprintf(stderr, "Error: can't find light uniform variables in class\n");
			return;
	    }
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -55.0f;
	
	offset = -20;
	lightposition[0] = lightposition[0] - position[0];
	lightposition[1] = lightposition[1] - position[1];

	if(position[2] > ((float)repeats2)*60 - offset){
		lightposition[2] += ((float)repeats2)*-60;
	}
	lightposition[2] = lightposition[2] + position[2];

	glUniform3fv(lightpositionHandle, 1, lightposition);
	//---------------------------------------------------


}



void Lighting::createLight(unsigned int id, float type, float* _direction, float* _position, float* _ambient, float* _diffuse, float* _specular, float _coneAngle){
	
	glUseProgram(id);
	std::string light = "light[" + std::to_string(lightCounter) + "]";

	float lightposition[3];
	float lightdirection[3];
	float lightambient[3];	// ambient light components
	float lightdiffuse[3];	// diffuse light components
	float lightspecular[3];	// specular light components
	float lighttype;
	float coneAngle;

    for(int i = 0 ; i<3 ; i++){
    	lightdirection[i] = _direction[i];	//light direction
    	lightposition[i] = _position[i];	// light position
		lightambient[i] = _ambient[i];		// ambient light components
		lightdiffuse[i] = _diffuse[i];		// diffuse light components
		lightspecular[i] = _specular[i];	// specular light components
    }

	lighttype = type;
	coneAngle = _coneAngle;


	if(type == 0){ //DIRECTIONAL LIGHT
		// /static_cast<const GLchar*>light + ".dirLight.direction"
		int lightdirectionHandle = glGetUniformLocation(id, &(light +".dirLight.direction")[0]);
		int lightambientHandle = glGetUniformLocation(id, &(light + ".dirLight.ambient")[0]);
		int lightdiffuseHandle = glGetUniformLocation(id, &(light + ".dirLight.diffuse")[0]);
		int lightspecularHandle = glGetUniformLocation(id, &(light + ".dirLight.specular")[0]);
		int lighttypeHandle = glGetUniformLocation(id, &(light + ".type")[0]);
		if ( lightambientHandle == -1 ||
	         lightdiffuseHandle == -1 ||
	         lightspecularHandle == -1 ||
	         lightdirectionHandle == -1 ||
	         lighttypeHandle == -1
	         ) {
	        fprintf(stderr, "Error: can't find light uniform variables in class\n");
			return;
	    }

		glUniform1f(lighttypeHandle, lighttype);
		glUniform3fv(lightdirectionHandle, 1, lightdirection);
		glUniform3fv(lightambientHandle, 1, lightambient);
		glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
		glUniform3fv(lightspecularHandle, 1, lightspecular); 
	}
	else if(type == 1){ //SPOT LIGHT
		int lightdirectionHandle = glGetUniformLocation(id, &(light +".spoLight.direction")[0]);
		int lightpositionHandle = glGetUniformLocation(id, &(light +".spoLight.position")[0]);
		int lightambientHandle = glGetUniformLocation(id, &(light + ".spoLight.ambient")[0]);
		int lightdiffuseHandle = glGetUniformLocation(id, &(light + ".spoLight.diffuse")[0]);
		int lightspecularHandle = glGetUniformLocation(id, &(light + ".spoLight.specular")[0]);
		int lightconeHandle = glGetUniformLocation(id, &(light + ".spoLight.coneAngle")[0]);
		int lighttypeHandle = glGetUniformLocation(id, &(light + ".type")[0]);
		if ( lightambientHandle == -1 ||
	         lightdiffuseHandle == -1 ||
	         lightspecularHandle == -1 ||
	         lightdirectionHandle == -1 ||
	         lightpositionHandle == -1 ||
	         lightconeHandle == -1 ||
	         lighttypeHandle == -1
	         ) {
	        fprintf(stderr, "Error: can't find light uniform variables\n");
			return;
	    }

		glUniform1f(lighttypeHandle, lighttype);
		glUniform1f(lightconeHandle, coneAngle);
		glUniform3fv(lightdirectionHandle, 1, lightdirection);
		glUniform3fv(lightpositionHandle, 1, lightposition);
		glUniform3fv(lightambientHandle, 1, lightambient);
		glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
		glUniform3fv(lightspecularHandle, 1, lightspecular); 
	}
	else{  //POINT LIGHT
		int lightpositionHandle = glGetUniformLocation(id, &(light +".poiLight.position")[0]);
		int lightambientHandle = glGetUniformLocation(id, &(light + ".poiLight.ambient")[0]);
		int lightdiffuseHandle = glGetUniformLocation(id, &(light + ".poiLight.diffuse")[0]);
		int lightspecularHandle = glGetUniformLocation(id, &(light + ".poiLight.specular")[0]);
		int lighttypeHandle = glGetUniformLocation(id, &(light + ".type")[0]);
		if ( lightambientHandle == -1 ||
	         lightdiffuseHandle == -1 ||
	         lightspecularHandle == -1 ||
	         lightpositionHandle == -1 ||
	         lighttypeHandle == -1
	         ) {
	        fprintf(stderr, "Error: can't find light uniform variables\n");
			return;
	    }

		glUniform1f(lighttypeHandle, lighttype);
		glUniform3fv(lightpositionHandle, 1, lightposition);
		glUniform3fv(lightambientHandle, 1, lightambient);
		glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
		glUniform3fv(lightspecularHandle, 1, lightspecular); 
	}

	lightCounter++;
}

void Lighting::setupLightUniforms(unsigned int id){
	//light 1 directional light
	float lightposition[3] = {0.0f, 0.0f, 0.0f};
	float lightdirection[3] = {2.0f, 1.0f, 1.0f};
	float lightambient[3] = {0.05f, 0.05f, 0.05f};	// ambient light components
	float lightdiffuse[3] = {0.3f, 0.3f, 0.3f};	// diffuse light components
	float lightspecular[3] = {0.05f, 0.05f, 0.05f};	// specular light components
	float lighttype = 0;
	float coneAngle = 0;
	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);

	//Copy for each new light and input new paramets
	lightposition[0] = 0.0f;
	lightposition[1] = 0.0f;
	lightposition[2] = 0.0f;

	lightdirection[0] = 0.0f;
	lightdirection[1] = 0.0f;
	lightdirection[2] = -1.0f;

	lightambient[0] = 0.05f;
	lightambient[1] = 0.05f;
	lightambient[2] = 0.05f;
	
	lightdiffuse[0] = 0.2f;
	lightdiffuse[1] = 0.2f;
	lightdiffuse[2] = 0.2f;

	lightspecular[0] = 0.05f;
	lightspecular[1] = 0.05f;
	lightspecular[2] = 0.05f;

	lighttype = 1;
	coneAngle = 5;

	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);

	//Light 2
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -5.0f;

	lightdirection[0] = -0.0f;
	lightdirection[1] = -0.2f;
	lightdirection[2] = -1.0f;

	lightambient[0] = 0.02f;
	lightambient[1] = 0.02f;
	lightambient[2] = 0.02f;
	
	lightdiffuse[0] = 0.5f;
	lightdiffuse[1] = 0.5f;
	lightdiffuse[2] = 0.5f;

	lightspecular[0] = 1.0f;
	lightspecular[1] = 1.0f;
	lightspecular[2] = 1.0f;

	lighttype = 2;
	coneAngle = 0;

	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);

			//Copy for each new light and input new paramets
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -15.0f;

	lightdirection[0] = -0.0f;
	lightdirection[1] = -0.2f;
	lightdirection[2] = -1.0f;

	lightambient[0] = 0.0f;
	lightambient[1] = 0.0f;
	lightambient[2] = 0.02f;
	
	lightdiffuse[0] = 0.0f;
	lightdiffuse[1] = 0.5f;
	lightdiffuse[2] = 0.0f;

	lightspecular[0] = 1.0f;
	lightspecular[1] = 1.0f;
	lightspecular[2] = 1.0f;

	lighttype = 2;
	coneAngle = 0;

	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);

			//Copy for each new light and input new paramets
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -25.0f;

	lightdirection[0] = -0.0f;
	lightdirection[1] = -0.2f;
	lightdirection[2] = -1.0f;

	lightambient[0] = 0.0f;
	lightambient[1] = 0.0f;
	lightambient[2] = 0.02f;
	
	lightdiffuse[0] = 0.5f;
	lightdiffuse[1] = 0.0f;
	lightdiffuse[2] = 0.0f;

	lightspecular[0] = 1.0f;
	lightspecular[1] = 1.0f;
	lightspecular[2] = 1.0f;

	lighttype = 2;
	coneAngle = 0;

	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);

			//Copy for each new light and input new paramets
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -35.0f;

	lightdirection[0] = -0.0f;
	lightdirection[1] = -0.2f;
	lightdirection[2] = -1.0f;

	lightambient[0] = 0.0f;
	lightambient[1] = 0.0f;
	lightambient[2] = 0.02f;
	
	lightdiffuse[0] = 0.0f;
	lightdiffuse[1] = 0.0f;
	lightdiffuse[2] = 0.5f;

	lightspecular[0] = 1.0f;
	lightspecular[1] = 1.0f;
	lightspecular[2] = 1.0f;

	lighttype = 2;
	coneAngle = 0;

	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);

			//Copy for each new light and input new paramets
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -45.0f;

	lightdirection[0] = -0.0f;
	lightdirection[1] = -0.2f;
	lightdirection[2] = -1.0f;

	lightambient[0] = 0.0f;
	lightambient[1] = 0.02f;
	lightambient[2] = 0.0f;
	
	lightdiffuse[0] = 0.0f;
	lightdiffuse[1] = 0.5f;
	lightdiffuse[2] = 0.0f;

	lightspecular[0] = 1.0f;
	lightspecular[1] = 1.0f;
	lightspecular[2] = 1.0f;

	lighttype = 2;
	coneAngle = 0;

	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);

			//Copy for each new light and input new paramets
	lightposition[0] = 1.0f;
	lightposition[1] = 1.0f;
	lightposition[2] = -55.0f;

	lightdirection[0] = -0.0f;
	lightdirection[1] = -0.2f;
	lightdirection[2] = -1.0f;

	lightambient[0] = 0.02f;
	lightambient[1] = 0.0f;
	lightambient[2] = 0.0f;
	
	lightdiffuse[0] = 0.5f;
	lightdiffuse[1] = 0.0f;
	lightdiffuse[2] = 0.0f;

	lightspecular[0] = 1.0f;
	lightspecular[1] = 1.0f;
	lightspecular[2] = 1.0f;

	lighttype = 2;
	coneAngle = 0;

	createLight(id, lighttype, lightdirection, lightposition, lightambient, lightdiffuse, lightspecular, coneAngle);
}
