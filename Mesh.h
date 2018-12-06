
/**
 This class draws a table by scaling and moving blocks.
 The table top is the plane y = 0
*/

class Mesh
{
    string filename;
	//int materialIndex;
    struct material_s {
    	float ambient[3];
    	float diffuse[3];
    	float specular[3];
    	float shininess;
    	string texName;
    	unsigned int textureID;
    } objMats[4];
    int matCount = 0;
    
public:
	unsigned int vaoHandle;
	unsigned int count;
	
    Mesh(string fileName, unsigned int id);
    int createVAO(int programID, string objFile);
    void render(int programID, glm::vec3 position, glm::vec3 scale);
    void setupMaterialUniforms(unsigned int id);
    void createTextures();
};
