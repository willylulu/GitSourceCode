#include"Texture.h"
#include "Render.h"
#define GLM_SWIZZLE
using namespace glm;
class HeightMap {
public:
	HeightMap(char* filename,float,float,float);
	float getHeight(float x, float y);
	void gravity(FbxEntity &a,float);
	void gravity(StaticObject &a,float);
	void gravity(FirstMan &firstMan,float);
	GLuint getProgram();
	GLuint getTexFbo();
	mat4 getVMatrix();
	mat4 getVMatrix(vec3* eyeposition,vec3 *forwardVector,vec3* upVector);
	mat4 getPMatrix();
	mat4 getVPMatrix();
	void startFbo();
	void endFbo();
	void reshape(float width, float height);
private:
	unsigned char *data;
	int width;
	int height;
	unsigned char *data2;
	int width2;
	int height2;
	float gravityValue;
	float mapWidth;
	float mapHeigh;
	GLuint Fbo;
	GLuint TexFbo;
	GLuint Rbo;
	GLuint program;
	mat4 Look;
	mat4 Othro;
};