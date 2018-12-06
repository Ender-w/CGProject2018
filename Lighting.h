class Lighting
{
private:
    int lightCounter;
public:
    Lighting(); // constructor
    void render(unsigned int id, float* player_pos); //Not Implemented

    //createLight is used to create a light source at 
    void createLight(unsigned int id, float type, float* _direction, float* _position, float* _ambient, float* _diffuse, float* _specular, float _coneAngle);
    void setupLightUniforms(unsigned int id);
};