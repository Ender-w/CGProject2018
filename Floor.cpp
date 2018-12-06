#include <GL/glew.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "external_files/stb_image.h"

#include "Floor.h"

/*
 * Floor.cpp. Generates VAO for floor as well as calculates bitangents, tangents and loads various textures.
 * Author Mark - a1672504
 */
 
Floor::Floor(unsigned int id){
	vaoHandle = createVAO(id);
	setupMaterialUniforms(id);
	diffuse_mapID = createDiffuseTexture();
	specular_mapID = createSpecularTexture();
	normal_mapID = createNormalTexture();
	depth_mapID = createDepthTexture();
}

unsigned int Floor::createVAO(unsigned int id){
	float Vertices[18] ={
		0.0, 0.0, 0.0,  //pos1 0,1,2
		0.0, 0.0, 1.0,	//pos2 3,4,5
		1.0, 0.0, 0.0,	//pos3 6,7,8
		0.0, 0.0, 1.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 1.0	//pos4 9,10,11
	};

	float Normals[18] = {
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0
		};

	//2 Trinagles 3 vertices 3 values
	float tex_coord[2*6] ={
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};
	
	 // Generating a square object
    float tangents[ 18 ];

    // Generating a square object
    float bitangents[ 18 ];


    float edge1[3] = {
    	Vertices[12] - Vertices[9],
    	Vertices[13] - Vertices[10],
    	Vertices[14] - Vertices[11]
    };

    float edge2[3] = {
    	Vertices[15] - Vertices[9],
    	Vertices[16] - Vertices[10],
    	Vertices[17] - Vertices[11]
    };

    float deltaUV1[2] = {
    	tex_coord[8] - tex_coord[6],
    	tex_coord[9] - tex_coord[7]
    };

    float deltaUV2[2] = {
    	tex_coord[10] - tex_coord[6],
    	tex_coord[11] - tex_coord[7]
    };

    float f1 = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

    glm::vec3 tangent1;
    glm::vec3 bitangent1;

    tangent1.x = f1 * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
	tangent1.y = f1 * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
	tangent1.z = f1 * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f1 * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
	bitangent1.y = f1 * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
	bitangent1.z = f1 * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);
	bitangent1 = glm::normalize(bitangent1);  


    float edge3[3] = {
    	Vertices[3] - Vertices[0],
    	Vertices[4] - Vertices[1],
    	Vertices[5] - Vertices[2]
    };

    float edge4[3] = {
    	Vertices[6] - Vertices[0],
    	Vertices[7] - Vertices[1],
    	Vertices[8] - Vertices[2]
    };

    float deltaUV3[2] = {
    	tex_coord[2] - tex_coord[0],
    	tex_coord[3] - tex_coord[1]
    };

    float deltaUV4[2] = {
    	tex_coord[4] - tex_coord[0],
    	tex_coord[5] - tex_coord[1]
    };

    float f2 = 1.0f / (deltaUV3[0] * deltaUV4[1] - deltaUV4[0] * deltaUV3[1]);

    glm::vec3 tangent2;
    glm::vec3 bitangent2;

    tangent2.x = f2 * (deltaUV4[1] * edge3[0] - deltaUV3[1] * edge4[0]);
	tangent2.y = f2 * (deltaUV4[1] * edge3[1] - deltaUV3[1] * edge4[1]);
	tangent2.z = f2 * (deltaUV4[1] * edge3[2] - deltaUV3[1] * edge4[2]);
	tangent2 = glm::normalize(tangent2);

	bitangent2.x = f2 * (-deltaUV4[0] * edge3[0] + deltaUV3[0] * edge4[0]);
	bitangent2.y = f2 * (-deltaUV4[0] * edge3[1] + deltaUV3[0] * edge4[1]);
	bitangent2.z = f2 * (-deltaUV4[0] * edge3[2] + deltaUV3[0] * edge4[2]);
	bitangent2 = glm::normalize(bitangent2);

	for(int i = 0; i < 18 ; i = i+3) {
		if(i<9){
			tangents[i] = tangent1.x;
			tangents[i+1] = tangent1.y;
			tangents[i+2] = tangent1.z;
			
			bitangents[i] = bitangent1.x;
			bitangents[i+1] = bitangent1.y;
			bitangents[i+2] = bitangent1.z;
		}
		else{
			tangents[i] = tangent2.x;
			tangents[i+1] = tangent2.y;
			tangents[i+2] = tangent2.z;
			
			bitangents[i] = bitangent2.x;
			bitangents[i+1] = bitangent2.y;
			bitangents[i+2] = bitangent2.z;
		}
	}

    glUseProgram(id);

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	unsigned int buffer[5];
	glGenBuffers(5, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

     glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Set tangent attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Set bitangent attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bitangents), bitangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	return vaoHandle;
}


void Floor::setupMaterialUniforms(unsigned int id){
	glUseProgram(id);
	int ambientHandle = glGetUniformLocation(id, "materials[0].m_ambient");
	int diffuseHandle = glGetUniformLocation(id, "materials[0].m_diffuse");
	int specularHandle = glGetUniformLocation(id, "materials[0].m_specular");
	int shininessHandle = glGetUniformLocation(id, "materials[0].m_shininess");
	if ( 	shininessHandle == -1 || 
			ambientHandle == -1 ||
			diffuseHandle == -1 ||
			specularHandle == -1) {
	        fprintf(stderr, "Error: can't find material uniform variables in floor\n");
	    }
	
	float ambient[3] = {1, 1, 1};
	float diffuse[3] = {1, 1, 1};
	float specular[3]= {1, 1, 1};
	float shininess = 32;

	glUniform3fv(ambientHandle, 1, ambient);
	glUniform3fv(diffuseHandle, 1, diffuse);
	glUniform3fv(specularHandle, 1, specular);
	glUniform1f(shininessHandle, shininess);
	
}

unsigned int Floor::createDiffuseTexture(){
	unsigned int diffuse_map;
	glGenTextures(1, &diffuse_map);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, diffuse_map);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//load tex
	int width, height, nrChannels;
	std::string textureName = "Stone_Floor_002_SD/Stone_Floor_002_COLOR.jpg";
	unsigned char *data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 3);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		
	}
	stbi_image_free(data);
	
	return diffuse_map;
}

unsigned int Floor::createSpecularTexture(){
	unsigned int specular_map;
	glGenTextures(1, &specular_map);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, specular_map);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//load tex
	int width, height, nrChannels;
	std::string textureName = "Stone_Floor_002_SD/Stone_Floor_002_SPEC.jpg";
	unsigned char *data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 3);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		
	}
	stbi_image_free(data);
	
	return specular_map;
}

unsigned int Floor::createNormalTexture(){
	unsigned int normal_map;
	glGenTextures(1, &normal_map);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, normal_map);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//load tex
	int width, height, nrChannels;
	std::string textureName = "Stone_Floor_002_SD/Stone_Floor_002_NORM.jpg";
	unsigned char *data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 3);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		
	}
	stbi_image_free(data);
	
	return normal_map;
}

unsigned int Floor::createDepthTexture(){
	unsigned int depth_map;
	glGenTextures(1, &depth_map);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//load tex
	int width, height, nrChannels;
	std::string textureName = "Stone_Floor_002_SD/Stone_Floor_002_DISP.png";
	unsigned char *data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 3);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		
	}
	stbi_image_free(data);
	
	return depth_map;
}

void Floor::render(unsigned int id, glm::vec3 player_pos, int toggle){
	glUseProgram(id);
	
	int floorHandle = glGetUniformLocation(id, "floor");
	glUniform1i(floorHandle, toggle);
	
	
	setupMaterialUniforms(id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_mapID);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, diffuse_mapID);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, specular_mapID);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, normal_mapID);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depth_mapID);

	glBindVertexArray(vaoHandle);
	//transform the object
	for(int j = -5 ; j < 40 ; j=j+3){
		for(int i = 0 ; i < 1 ; i++){
			int modelUniformHandle = glGetUniformLocation(id, "model");
			int normHandle = glGetUniformLocation(id, "normal_matrix");
			if (modelUniformHandle == -1 ||
				normHandle == -1){
				fprintf(stderr, "Error: Floor Rendering can't find uniforms\n");
				exit(1);
			}
		
		    glm::mat4 model;
		    glm::mat3 normMatrix;
		    normMatrix = glm::mat3(model);

		    float x = i - 1.5f - (float)player_pos.x;
		    float y = -1.5f - (float)player_pos.y; 
		    float z = -j + ((float)player_pos.z - (int)player_pos.z) + (int)player_pos.z%3;

			model = glm::translate(model, glm::vec3(x, y, z));
			model = glm::scale(model, glm::vec3(3,3,3));

			glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
			glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));

			glDrawArrays(GL_TRIANGLES, 0, 3*3*2); 
	    	//glDrawElements(GL_TRIANGLES,6 ,GL_UNSIGNED_INT, reinterpret_cast<void*>(0)); 
		

		}
	}
	


    
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glFlush();
}

