#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "external_files/FastNoise.h"

using namespace std;

#include "Mesh.h"
#include "ProceduralGenerator.h"
/**
 * Procedural geometry and texture generation
 * Will endlessly create terrain on demand
 * Uses external file FastNoise.h to generate the noise
 * Creates 2 buffers of mesh data and swaps between them to achieve seamless terrain
 * Author Daniel - a1688639
 */
ProceduralGenerator::ProceduralGenerator(int meshSize){
	size = meshSize;
	noiseGenerator.SetNoiseType(FastNoise::Simplex);
	noiseGenerator.SetFrequency(0.09);
	textureGenerator.SetNoiseType(FastNoise::Value);
	textureGenerator.SetFrequency(0.3);
	
	//Create mesh data
	//segment 1
	vector<float> vertexData = CreateVertexData(0);
	
    glGenVertexArrays(1, &vaoHandle1);
    glGenBuffers(1, &vbo1);
    
    glBindVertexArray(vaoHandle1);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertexData[0]), vertexData.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0*sizeof(float)));
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	treePositions1 = GenerateTreePositions(size);
	
	//segment 2
	vertexData = CreateVertexData(size);
	
    glGenVertexArrays(1, &vaoHandle2);
    glGenBuffers(1, &vbo2);
    
    glBindVertexArray(vaoHandle2);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertexData[0]), vertexData.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0*sizeof(float)));
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	treePositions2 = GenerateTreePositions(size);
	
	chunkOffset = size;
    
    //Mesh data done
    //Create procedural Texture
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	unsigned char r1, g1, b1, r2, g2, b2;
	r1 = 51;
	g1 = 26;
	b1 = 0;
	r2 = 0;
	g2 = 133;
	b2 = 0;
	
	int dimensions = 128;
	unsigned char *data = new unsigned char[dimensions*dimensions*3];
	for(int x = 0; x < dimensions; x++){
		for(int y = 0; y < dimensions; y++){
			float lerpFactor = (textureGenerator.GetNoise(x,y) + 1)/2;
			data[(x*dimensions*3) + (y*3) + 0] = (r2 - r1) * lerpFactor + r1;//r
			data[(x*dimensions*3) + (y*3) + 1] = (g2 - g1) * lerpFactor + g1;//g
			data[(x*dimensions*3) + (y*3) + 2] = (b2 - b1) * lerpFactor + b1;//b
		}
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dimensions, dimensions, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	delete data;
	
}

vector<vector<int>> ProceduralGenerator::GenerateTreePositions(int meshsize){
	int treeCount = 10;
	vector<vector<int>> treePositions;
	for(int i = 0; i < treeCount; i++){
		int x = rand() % meshsize;//tree coords
		int y = rand() % meshsize;
		int z = rand() % 2 + 1;//tree type
		//no duplicate trees
		while(treeExists(treePositions, x, y)){
			x = rand() % meshsize;
			y = rand() % meshsize;
		}
		//limit trees from center road
		while((x > meshsize/2-3 && x < meshsize/2+3) || treeExists(treePositions, x, y)){
			x = rand() % meshsize;
		}
		
		vector<int> tree;
		tree.push_back(x);
		tree.push_back(y);
		tree.push_back(z);
		treePositions.push_back(tree);
	}
	return treePositions;
}

bool ProceduralGenerator::treeExists(vector<vector<int>> trees, int x, int y){
	for(int i = 0; i < trees.size(); i++){
		if(trees[i][0] == x && trees[i][1] == y){
			return true;
		}
	}
	return false;
}

//Creates a new batch of vertex data based on the offset from the start
vector<float> ProceduralGenerator::CreateVertexData(float offset){
	vector<float> vertexData;
	for(int y = 0; y < size; y++){
		for(int x = 0; x < size; x++){
			if(x != size-1){
				count += 3;
				glm::vec3 a((float)x, noiseGenerator.GetNoise(x,y-offset), (float)y);
				glm::vec3 b((float)x+1, noiseGenerator.GetNoise(x+1,y-offset), (float)y);
				glm::vec3 c((float)x, noiseGenerator.GetNoise(x,y+1-offset), (float)y+1);
				glm::vec3 dir = glm::cross((b-a), (c-a));
				glm::vec3 normal = dir/glm::length(dir);
				glm::vec3 temp = a;
				glm::vec2 texCoord(0, 1);
				for(int i = 0; i < 3; i++){
					if(i == 1){
						temp = b;
						texCoord = glm::vec2(1,1);
					}else if(i == 2){
						temp = c;
						texCoord = glm::vec2(0,0);
					}
					vertexData.push_back(temp.x);
					vertexData.push_back(temp.y);
					vertexData.push_back(temp.z);
					vertexData.push_back(normal.x);
					vertexData.push_back(normal.y);
					vertexData.push_back(normal.z);
					vertexData.push_back(texCoord.x);
					vertexData.push_back(texCoord.y);
				}
			}
			if(x != 0){
				count += 3;
				glm::vec3 a((float)x, noiseGenerator.GetNoise(x,y-offset), (float)y);
				glm::vec3 b((float)x, noiseGenerator.GetNoise(x,y+1-offset), (float)y+1);
				glm::vec3 c((float)x-1, noiseGenerator.GetNoise(x-1,y+1-offset), (float)y+1);
				glm::vec3 dir = glm::cross((b-a), (c-a));
				glm::vec3 normal = dir/glm::length(dir);
				glm::vec3 temp = a;
				glm::vec2 texCoord(1, 1);
				for(int i = 0; i < 3; i++){
					if(i == 1){
						temp = b;
						texCoord = glm::vec2(1,0);
					}else if(i == 2){
						temp = c;
						texCoord = glm::vec2(0,0);
					}
					vertexData.push_back(temp.x);
					vertexData.push_back(temp.y);
					vertexData.push_back(temp.z);
					vertexData.push_back(normal.x);
					vertexData.push_back(normal.y);
					vertexData.push_back(normal.z);
					vertexData.push_back(texCoord.x);
					vertexData.push_back(texCoord.y);
				}
			}
		}
	}
	return vertexData;
}

void ProceduralGenerator::setMaterialUniforms(unsigned int id){
	glUseProgram(id);
	
	int ambientHandle = glGetUniformLocation(id, "materials[0].m_ambient");
	int diffuseHandle = glGetUniformLocation(id, "materials[0].m_diffuse");
	int specularHandle = glGetUniformLocation(id, "materials[0].m_specular");
	int shininessHandle = glGetUniformLocation(id, "materials[0].m_shininess");
	
	if (shininessHandle == -1 || 
		ambientHandle == -1 ||
		diffuseHandle == -1 ||
		specularHandle == -1) {
	    fprintf(stderr, "Error: material uniform failed in procedural\n");
	}
	    
	float ambient[3] = {0.5, 0.5, 0.5};
	float diffuse[3] = {0.5, 0.5, 0.5};
	float specular[3]= {0.5, 0.5, 0.5};
	float shininess = 16;
	    
	glUniform3fv(ambientHandle, 1, ambient);
	glUniform3fv(diffuseHandle, 1, diffuse);
	glUniform3fv(specularHandle, 1, specular);
	glUniform1f(shininessHandle, shininess);
}

void ProceduralGenerator::render(int programID, glm::vec3 playerPos, Mesh *tree1, Mesh *tree2)
{
	glUseProgram(programID);
	
	int modelUniformHandle = glGetUniformLocation(programID, "model");
	if (modelUniformHandle == -1)
		exit(1);
    glm::mat4 model;
	
	//render buffer 1
	setMaterialUniforms(programID);
	glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(vaoHandle1);
    model = glm::translate(model, glm::vec3(-(size/2),-2.5,-chunkOffset+10));//
    model = glm::translate(model, glm::vec3(-playerPos.x,-playerPos.y,playerPos.z));
    glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawArrays(GL_TRIANGLES, 0, count);
    
    //render trees 1
    for(int i = 0; i < treePositions1.size(); i++){
    	glm::vec3 position = glm::vec3(-(size/2), -2.5, -chunkOffset + 10);
    	position.x = position.x + treePositions1[i][0] - playerPos.x;
    	position.y = position.y + noiseGenerator.GetNoise(treePositions1[i][0], treePositions1[i][1]-chunkOffset+size)-playerPos.y;
    	position.z = position.z + treePositions1[i][1] + playerPos.z;
    	if(treePositions1[i][2] == 1){//tree type 1
    		tree1->render(programID, position, glm::vec3(2,2,2));
    	}else{//tree type 2
    		tree2->render(programID, position, glm::vec3(1,1,1));
    	}
    }
    
    //render buffer 2
    setMaterialUniforms(programID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(vaoHandle2);
    model = glm::translate(model, glm::vec3(0,0,-size));
    glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawArrays(GL_TRIANGLES, 0, count);
    
    //render trees 2
    for(int i = 0; i < treePositions2.size(); i++){
    	glm::vec3 position = glm::vec3(-(size/2), -2.5, -chunkOffset + 10 - size);
    	position.x = position.x + treePositions2[i][0] - playerPos.x;
    	position.y = position.y + noiseGenerator.GetNoise(treePositions2[i][0], treePositions2[i][1]-chunkOffset)-playerPos.y;
    	position.z = position.z + treePositions2[i][1] + playerPos.z;
    	if(treePositions2[i][2] == 1){//tree type 1
    		tree1->render(programID, position, glm::vec3(2,2,2));
    	}else{//tree type 2
    		tree2->render(programID, position, glm::vec3(1,1,1));
    	}
    }
    
    //if vertex data needs an update
    if(playerPos.z > chunkOffset-10){
    	chunkOffset += size;
    	//swap the buffers
    	unsigned int temp = vaoHandle1;
    	vaoHandle1 = vaoHandle2;
    	vaoHandle2 = temp;
    	temp = vbo1;
    	vbo1 = vbo2;
    	vbo2 = temp;
    	//swap trees
    	treePositions1.clear();
    	treePositions1 = treePositions2;
    	treePositions2.clear();
    	treePositions2 = GenerateTreePositions(size);
    	
    	//update the vertex data on the gpu
    	vector<float> vertexData = CreateVertexData(chunkOffset);
    	
		glBindVertexArray(vaoHandle2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo2);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertexData[0]), vertexData.data(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
    	
    }
    
    glBindVertexArray(0);
	glFlush();
}


