class Floor{
private:

public:
	unsigned int vaoHandle;
	unsigned int diffuse_mapID;
	unsigned int specular_mapID;
	unsigned int normal_mapID;
	unsigned int depth_mapID;

	Floor(unsigned int id);
	unsigned int createVAO(unsigned int id);
	unsigned int createDiffuseTexture();
	unsigned int createSpecularTexture();
	unsigned int createNormalTexture();
	unsigned int createDepthTexture();
	void setupMaterialUniforms(unsigned int id);
	void render(unsigned int id, glm::vec3 player_pos, int toggle);
};