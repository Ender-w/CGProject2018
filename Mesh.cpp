#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "external_files/stb_image.h"

using namespace std;

#include "external_files/tiny_obj_loader.h"
#include "Mesh.h"

/**
 * Mesh loading object. File name is passed in constructor
 * Objects position and scale are passed in the render function
 * Supports up to 4 materials on object
 * Author Daniel - a1688639
 */
int Mesh::createVAO(int programID, string objFile)
{
    // Cube vertex data
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    count = 0;
    
    string err;
    string materialDirectory = "models/";
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFile.c_str(), materialDirectory.c_str());
	
	
	if(!err.empty()){
		cerr << err << endl;
	}
	if(!ret){
		exit(1);
	}
    vector<float> vertexData;
    for(size_t s = 0; s < shapes.size(); s++){
    	size_t index_offset = 0;
    	for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++){
    		int fv = shapes[s].mesh.num_face_vertices[f];
    		float materialID = shapes[s].mesh.material_ids[f];
    		//loop over verts in faces
    		for(size_t v = 0; v < fv; v++){
    			tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
				vertexData.push_back(vx);

				tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
				vertexData.push_back(vy);

				tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
				vertexData.push_back(vz);

				tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
				vertexData.push_back(nx);
				tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
				vertexData.push_back(ny);
				tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
				vertexData.push_back(nz);
				
				if(((int)(attrib.texcoords.size())/2) == 0){//Check if the model actually has a texture
					vertexData.push_back(0);
					vertexData.push_back(0);
				}else{
					tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
					vertexData.push_back(tx);
					tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
					vertexData.push_back(ty);
				}
				count += 1;
				if(materialID > 4.1){
					materialID = 4;
				}
				vertexData.push_back(materialID);
				
    		}
    		index_offset += fv;
    	}
    }
    if(materials.size() > 4){
    	cout << "Above max supported materials for object" << endl;
    }
    //get material information
    matCount = materials.size();
    if(matCount > 4)
    	matCount = 4;
    for(int i = 0; i < matCount; i++){//for each material
    	objMats[i].texName = materials[i].diffuse_texname;
    	for(int x = 0; x < 3; x++){//loop through rgb
    		objMats[i].ambient[x] = materials[i].ambient[x];
    		objMats[i].diffuse[x] = materials[i].diffuse[x];
    		objMats[i].specular[x] = materials[i].specular[x];
    	}
    	objMats[i].shininess = materials[i].shininess;
    }
    
    unsigned int vbo;
    
    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vaoHandle);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertexData[0]), vertexData.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0*sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8*sizeof(float)));
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
	return vaoHandle;
}

  
Mesh::Mesh(std::string fileName, unsigned int id)
{

	
    vaoHandle = createVAO(id, fileName);
    
    createTextures();
}  

void Mesh::setupMaterialUniforms(unsigned int id){
	glUseProgram(id);
	for(int i = 0 ; i < matCount; i++){
		string uniformName = "materials[" + std::to_string(i) + "].";
		int ambientHandle = glGetUniformLocation(id, &(uniformName + "m_ambient")[0]);
		int diffuseHandle = glGetUniformLocation(id, &(uniformName + "m_diffuse")[0]);
		int specularHandle = glGetUniformLocation(id, &(uniformName + "m_specular")[0]);
		int shininessHandle = glGetUniformLocation(id, &(uniformName + "m_shininess")[0]);
		
		if ( 	shininessHandle == -1 || 
			ambientHandle == -1 ||
			diffuseHandle == -1 ||
			specularHandle == -1) {
	        fprintf(stderr, "Error: material uniform failed in mesh.cpp\n");
	    }
	    
		glUniform3fv(ambientHandle, 1, objMats[i].ambient);
		glUniform3fv(diffuseHandle, 1, objMats[i].diffuse);
		glUniform3fv(specularHandle, 1, objMats[i].specular);
		glUniform1f(shininessHandle, objMats[i].shininess);
	}
	
}


void Mesh::createTextures(){
	
	for(int i = 0; i < matCount; i++){
		glGenTextures(1, &objMats[i].textureID);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, objMats[i].textureID);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		//load tex
		int width, height, nrChannels;
		string textureName = "models/" + objMats[i].texName;
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
	}
	
	
}

/**
 Draw the mesh
*/  
void Mesh::render(int programID, glm::vec3 position, glm::vec3 scale)
{
	glUseProgram(programID);
	
	int modelUniformHandle = glGetUniformLocation(programID, "model");
	if (modelUniformHandle == -1)
		exit(1);
	int normHandle = glGetUniformLocation(programID, "normal_matrix");
	
	setupMaterialUniforms(programID);
	
	glBindVertexArray(vaoHandle);
	
	for(int i = 0; i < matCount; i++){
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, objMats[i].textureID);
	}
	
    glm::mat4 model;
    glm::mat3 normMatrix;
    normMatrix = glm::mat3(model);
	  
	//transform the object
	
	model = glm::translate(model, position);
	model = glm::scale( model, scale );
	
	glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    glDrawArrays(GL_TRIANGLES, 0, count);  

    glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);

	glFlush();
    
}

