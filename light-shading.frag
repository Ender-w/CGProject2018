#version 330

/**
 * Light-shading.frag. Caclculates all lighting effects and various lighting techniques on the floor
 * Author Mark - a1672504
 */

in vec4 vertex;
in vec3 normal;
in vec2 texCoord;
in float materialID;

in mat3 TBN;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

// The final colour we will see at this location on screen
out vec4 fragColour;

struct MaterialStruct {
	float m_shininess;
	vec3 m_ambient;
	vec3 m_diffuse;
	vec3 m_specular;
};

uniform MaterialStruct materials[4];


vec3 mtl_ambient = vec3(0,0,0);       // Ambient surface colour
vec3 mtl_diffuse = vec3(0,0,0);       // Diffuse surface colour  
vec3 mtl_specular = vec3(0,0,0);      // Specular surface colour
float shininess = 0;    	// Material surface shininess
vec3 textureVector = vec3(0,0,0);

uniform sampler2D texMap1;
uniform sampler2D texMap2;
uniform sampler2D texMap3;
uniform sampler2D texMap4;
uniform sampler2D diffuse_map; //Diffuse Map
uniform sampler2D specular_map;
uniform sampler2D normal_map;
uniform sampler2D depth_map;

uniform int floor; // if rendering floor use maps for material
uniform int lightToggle;

struct directionalLight {
    vec3 direction;     // Light direction
    
    vec3 ambient;       // Light ambient RGBA values
    vec3 diffuse;       // Light diffuse RGBA values
    vec3 specular;      // Light specular RGBA values
};

struct spotLight {
    vec3 position;      // Light position
    vec3 direction;     // Light direction
    
    vec3 ambient;       // Light ambient RGBA values
    vec3 diffuse;       // Light diffuse RGBA values
    vec3 specular;      // Light specular RGBA values
    
    float coneAngle;    // Cone angle for spotlight
};

struct pointLight {
    vec3 position;      // Light position
    
    vec3 ambient;       // Light ambient RGBA values
    vec3 diffuse;       // Light diffuse RGBA values
    vec3 specular;      // Light specular RGBA values
};

struct Light {
    float type;                 // 0 = dirLight, 1 = spotLight, 2 = pointLight

    directionalLight dirLight;    //directional light source
    spotLight spoLight;         //spotlight light source
    pointLight poiLight;        //pointlight light source
};

#define NUMBER_LIGHTS 8
uniform Light light[NUMBER_LIGHTS];

vec2 ParallaxMapping(vec2 tex, vec3 viewDir){
	float height = texture(depth_map, tex).r;
	vec2 p = viewDir.xy / viewDir.z * (height/100); //CHANGE THE 100 to adjust parallax mapping not sure what is good
	return tex - p;
}

vec3 calcLight(Light light_val, vec3 position, vec3 norm, vec2 texST){
    vec3 result;
    if(light_val.type == 0){
        //Directional Light
        vec3 lightDirection = normalize(-light_val.dirLight.direction);
        
        //diffuse Constant 
        float diffuseC = max(dot(norm, lightDirection),0.0);

        //Specular Constant
        vec3 viewDirection = normalize(vec3(0.0,0.0,0.0)-position.xyz);
        vec3 reflection = reflect(-lightDirection,norm);
        float specularC = pow(max(dot(viewDirection,reflection),0.0), shininess);
        
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        //Light Values
        if(floor == 0){
            ambient = mtl_ambient * light_val.dirLight.ambient * textureVector;
            diffuse = mtl_diffuse * light_val.dirLight.diffuse * diffuseC * textureVector;
            specular = mtl_specular * light_val.dirLight.specular * specularC * textureVector;
        }
        else{
            ambient = light_val.dirLight.ambient * texture(diffuse_map, texST).xyz;
            diffuse = light_val.dirLight.diffuse * diffuseC * texture(diffuse_map, texST).xyz;
            specular = light_val.dirLight.specular * specularC * texture(specular_map, texST).xyz; 
        }
        
        result = ambient + diffuse + specular;
    }
    else if(light_val.type == 1){
        //Spot Light
        vec3 lightDirection;
		if(floor == 0){
			lightDirection = normalize(light_val.spoLight.position.xyz - position.xyz);
		}else{
			lightDirection = normalize(TBN*light_val.spoLight.position.xyz - position.xyz);
		}
        
        float angleFragment = dot(lightDirection, normalize(-light_val.spoLight.direction));
        float angleTorch = cos(radians(light_val.spoLight.coneAngle));
        float angleTorchOuter = cos(radians(light_val.spoLight.coneAngle + 5));
        float intensity = clamp((angleFragment - angleTorchOuter) / (angleTorch - angleTorchOuter), 0.0 , 1.0);

        
        //diffuse Constant 
        float diffuseC = max(dot(norm, lightDirection),0.0);

        //Specular Constant
        vec3 viewDirection = normalize(vec3(0.0,0.0,0.0)-position.xyz);
        vec3 reflection = reflect(-lightDirection,norm);
        float specularC = pow(max(dot(viewDirection,reflection),0.0), shininess);
        
        //Light Values
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        //Light Values
        if(floor == 0){
            ambient = mtl_ambient * light_val.spoLight.ambient * textureVector;
            diffuse = mtl_diffuse * light_val.spoLight.diffuse * diffuseC * textureVector;
            specular = mtl_specular * light_val.spoLight.specular * specularC * textureVector;
        }
        else{
            ambient = light_val.spoLight.ambient * texture(diffuse_map, texST).xyz;
            diffuse = light_val.spoLight.diffuse * diffuseC * texture(diffuse_map, texST).xyz;
            specular = light_val.spoLight.specular * specularC * texture(specular_map, texST).xyz; 
        }
        
        diffuse *= intensity;
        specular *= intensity;

        float AttenuationFactor = 0.01;
        float distance = length(light_val.poiLight.position.xyz - position.xyz);
        float attenuation = 1.0 / (1.0 + AttenuationFactor*(pow(distance,2.0)));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        result = ambient + diffuse + specular;
    }
    else{
        //Point Light
        vec3 lightDirection;
        if(floor == 0){
        	lightDirection = normalize(light_val.spoLight.position.xyz - position.xyz);
        }else{
        	lightDirection = normalize(TBN*light_val.spoLight.position.xyz - position.xyz);
        }
        
        //diffuse Constant 
        float diffuseC = max(dot(norm, lightDirection),0.0);

        //Specular Constant
        vec3 viewDirection = normalize(vec3(0.0,0.0,0.0)-position.xyz);
        vec3 reflection = reflect(-lightDirection,norm);
        float specularC = pow(max(dot(viewDirection,reflection),0.0), shininess);
        
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        //Light Values
        if(floor == 0){
            ambient = mtl_ambient * light_val.poiLight.ambient * textureVector;
            diffuse = mtl_diffuse * light_val.poiLight.diffuse * diffuseC * textureVector;
            specular = mtl_specular * light_val.poiLight.specular * specularC * textureVector;
        }
        else{
            ambient = light_val.poiLight.ambient * texture(diffuse_map, texST).xyz;
            diffuse = light_val.poiLight.diffuse * diffuseC * texture(diffuse_map, texST).xyz;
            specular = light_val.poiLight.specular * specularC * texture(specular_map, texST).xyz; 
        }

        float AttenuationFactor = 0.07;
        float distance;
        if(floor == 0){
        	distance = length(light_val.poiLight.position.xyz - position.xyz);
        }else{
        	distance = length(TBN*light_val.poiLight.position.xyz - position.xyz);
        }
        float attenuation = 1.0 / (1.0 + AttenuationFactor*(pow(distance,2.0)));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *=  attenuation;
        
        result = ambient + diffuse + specular;
    }


    return result;
}

float fog_calc(in vec4 position)
{
    float dist = length(vec3(0.0,0.0,0.0) - position.xyz);
    dist = max(dist - 10,0.0f);
    dist = dist / 16 ; 
    float fog_factor = dist;
    if(fog_factor > 1){
        fog_factor = 1.0f;
    }
    return fog_factor;
}

//function for dealing with multiple materials in an object - Daniel
void setMaterials(){
	if(materialID < 0.2){//material 0
		mtl_ambient = materials[0].m_ambient;
		mtl_diffuse = materials[0].m_diffuse;
		mtl_specular = materials[0].m_specular;
		shininess = materials[0].m_shininess;
		textureVector = texture(texMap1, texCoord).xyz;
		//fragColour = vec4(0.956, 0.258, 0.258, 1);
	}else if(materialID > 0.8 && materialID < 1.2){//material 1
		mtl_ambient = materials[1].m_ambient;
		mtl_diffuse = materials[1].m_diffuse;
		mtl_specular = materials[1].m_specular;
		shininess = materials[1].m_shininess;
		textureVector = texture(texMap2, texCoord).xyz;
	}else if(materialID > 1.8 && materialID < 2.2){//material 2
		mtl_ambient = materials[2].m_ambient;
		mtl_diffuse = materials[2].m_diffuse;
		mtl_specular = materials[2].m_specular;
		shininess = materials[2].m_shininess;
		textureVector = texture(texMap3, texCoord).xyz;
	}else if(materialID > 2.8 && materialID < 3.2){//material 3
		mtl_ambient = materials[3].m_ambient;
		mtl_diffuse = materials[3].m_diffuse;
		mtl_specular = materials[3].m_specular;
		shininess = materials[3].m_shininess;
		textureVector = texture(texMap4, texCoord).xyz;
	}else{
		mtl_ambient = materials[0].m_ambient;
		mtl_diffuse = materials[0].m_diffuse;
		mtl_specular = materials[0].m_specular;
		shininess = materials[0].m_shininess;
		textureVector = texture(texMap1, texCoord).xyz;
		
	}
}

void main(void)
{
	
	vec3 colour = vec3(0.0f,0.0f,0.0f);
	
	setMaterials();
    vec4 fog = vec4(0.2f,0.2f,0.2f,1.0f);
    float k = fog_calc(vertex);
    
    vec3 normalStore;
    if(floor == 1){
    	//Parallax Mapping
    	vec3 viewDir = normalize(vec3(0.0,0.0,0.0) - tangentFragPos);
    	vec2 tex = ParallaxMapping(texCoord,viewDir);
    	
    	//Normal Mapping
    	normalStore = texture(normal_map,tex).xyz;
    	normalStore = TBN*normalize(normalStore*2.0 - 1.0);
    	
    	if(lightToggle == 1){
    		for(int i = 1 ; i < NUMBER_LIGHTS ; i++ ){
    			colour += calcLight(light[i],tangentFragPos,normalize(normalStore),tex);
    		}
    	}else{
    		colour += calcLight(light[0],tangentFragPos,normalize(normalStore),tex);
    	}
    }else{
    	normalStore = normal;
    	if(lightToggle == 1){
    		for(int i = 1 ; i < NUMBER_LIGHTS ; i++ ){
    			colour += calcLight(light[i],vertex.xyz,normalize(normalStore),texCoord);
    		}
    	}else{
    		colour += calcLight(light[0],vertex.xyz,normalize(normalStore),texCoord);
    	}
    }


    float gamma = 2.2;

    colour = pow(colour,vec3(1.0/gamma));

    fragColour = k*fog+(1-k)*vec4(colour,1.0);
}
