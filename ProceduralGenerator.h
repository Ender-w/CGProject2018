#include "external_files/FastNoise.h"

class ProceduralGenerator
{

	unsigned int vaoHandle1, vaoHandle2;
	unsigned int vbo1, vbo2;
	unsigned int texID;
	FastNoise noiseGenerator;
	FastNoise textureGenerator;
	float size;
	unsigned int count = 0;
	float chunkOffset;
	vector< vector<int> > treePositions1;
	vector< vector<int> > treePositions2;
	
public:
	
    ProceduralGenerator(int meshSize);
	void render(int programID, glm::vec3 playerPos, Mesh *tree1, Mesh *tree2);
	vector<float> CreateVertexData(float offset);
	vector<vector<int>> GenerateTreePositions(int size);
	void setMaterialUniforms(unsigned int id);
	bool treeExists(vector<vector<int>> trees, int x, int y);
};
