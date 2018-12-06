
/**
 This class draws a table by scaling and moving blocks.
 The table top is the plane y = 0
*/

class Skybox
{
    
    
public:
	unsigned int vaoHandle;
	unsigned int cubemapTexture;

    Skybox();
    void render(int programID, glm::mat4 viewMtx);
	unsigned int loadCubemap();
	unsigned int createVAO();
};
