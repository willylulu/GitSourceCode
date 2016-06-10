#include"Texture.h"
#include <glew.h>
#include <freeglut.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
class SkyBox {
public:
	SkyBox() {};
	SkyBox(char**);
	GLuint getVAO();
	GLuint getTexture();
	GLuint getSkyProgram();
	GLfloat* getVertex();
	void genStarCubeMap();
	void displaySkyBox(mat4*, mat4*, vec3*);
	void bindVP_cP(mat4*, mat4*, vec3*);
	void switchSkyBox();
private:
	GLuint cubemapTexture;
	GLuint starCubeMap;
	GLuint skyboxVAO;
	GLuint skyProgram;
	GLfloat* skyVertexs;
	mat4* matrixV;
	mat4* matrixP;
	vec3* cameraPosition;
	bool switchskybox;
};
