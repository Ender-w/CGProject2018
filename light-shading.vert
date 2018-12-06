
#version 330

// Position and colour variables. 1 per vertex.
layout (location = 0) in vec3 a_vertex; 
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoords;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in float a_materialID;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_matrix;

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle
out vec3 tangentViewPos;
out vec3 tangentFragPos;
out vec4 vertex; // vertex position in eye space
out vec3 normal; // the eye space normal
out vec2 texCoord;
out float materialID;
out mat3 TBN;

void main(void)
{
	vertex = model * vec4(a_vertex, 1.0);
	normal = normalize(normal_matrix * a_normal);
	
	vec3 T = normalize(vec3(model * vec4(a_tangent,   0.0)));
	vec3 B = normalize(vec3(model* vec4(a_bitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(normal,    0.0)));
	TBN = transpose(mat3(T,B, N));
	tangentViewPos = TBN * vec3(0.0,0.0,0.0);
	tangentFragPos = TBN * vertex.xyz;
	
	texCoord = a_texcoords;
	materialID = a_materialID;
	//gl_Position = projection * view * model * vec4(a_vertex, 1.0);
	gl_Position = projection * view * vertex;
}
