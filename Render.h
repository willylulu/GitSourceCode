#include "Parser.h"
#include "ShaderObject.h"
#include <algorithm>
#include "firstman.h"
struct UniformData {
	char *variable;
	char *type;
	int amount;
	void *data;
};

class StaticObject : public ObjParser {
public:
	bool onAir;
	vec3 speed;
	vec3 position;
	vec3 rotation;
	vec3 scale;
	mat4 mMatrix;
	StaticObject(char*, vec3, vec3, vec3);
	void render(ShaderObject*, std::vector<UniformData>);
private:
	void assignUniformData(char*, int, int*, ShaderObject*);
	void assignUniformData(char*, int, float*, ShaderObject*);
	void assignUniformData(char*, vec3&, ShaderObject*);
	void assignUniformData(char*, vec4&, ShaderObject*);
	void assignUniformData(char*, mat4&, ShaderObject*);
	void setUniformData(std::vector<UniformData>, ShaderObject*);
	void calculateModelMatrix();
};

struct time_lapse {
	float start;
	float end;
};

class FbxEntity : public FbxParser {
public:
	bool onAir;
	vec3 speed;
	vec3 position;
	vec3 rotation;
	vec3 scale;
	mat4 mMatrix;
	std::vector<GLuint> uniform;
	std::vector<char*> uniformReference;
	std::vector<time_lapse> animation_lapse;
	float time_animate;
	FbxEntity(char*, vec3, vec3, vec3);
	void save_animation(std::vector<float>);
	void do_animation(int);
	void render(ShaderObject*, std::vector<UniformData>);
	mat4 getModelMatrix();
	float animate_stop = 0;
private:
	void assignUniformData(char*, int, int*, ShaderObject*);
	void assignUniformData(char*, int, float*, ShaderObject*);
	void assignUniformData(char*, vec3&, ShaderObject*);
	void assignUniformData(char*, vec4&, ShaderObject*);
	void assignUniformData(char*, mat4&, ShaderObject*);
	void setUniformData(std::vector<UniformData>, ShaderObject*);
	void calculateModelMatrix();
};

static const float waterVertexs[] = {
	-53.12499,25,27.77779,
	-65.5,25,-26.388884,
	7.81,25,59.722183,

	-65.5,25,-26.388884,
	-66.406,25,-73.611137,
	7.81,25,59.722183,

	-66.406,25,-73.611137,
	-66.406250,25,-73.611137,
	7.81,25,59.722183,

	-66.406250,25,-73.611137,
	-56.250008,25,-95.833359,
	7.81,25,59.722183,

	-56.250008,25,-95.833359,
	32.031239,25,-123.611092,
	7.81,25,59.722183,

	32.031239,25,-123.611092,
	60.937523,25,-131.944412,
	7.81,25,59.722183,

	60.937523,25,-131.944412,
	176.562485,25,-15.277803,
	7.81,25,59.722183,

	176.562485,25,-15.277803,
	185.156219,25,19.444466,
	7.81,25,59.722183,

	185.156219,25,19.444466,
	177.343719,25,40.277779,
	7.81,25,59.722183,

	177.343719,25,40.277779,
	33.593773,25,76.388893,
	7.81,25,59.722183,
};

class Water {
public:
	Water(float speed, char* vs2, char* fs2);
	void displayWater(mat4 &um4mvp, mat4 &um4v, float time);
	void reshpae(float width, float height);
	void startFbo();
	void endFbo();
	void startFbo2();
	void endFbo2();
	GLuint getFboTex();
	GLuint getFboTex2();
	void genRenderProgram(char* vs2, char* fs2);
	GLuint getRenderProgram();
	void setChange(float time);
	mat4 getWaterReflectMatrix();
	void* getPtrWaterRefMat();
	vec3 getWaterReflectPosition();
	void* getPtrWaterRefPos();
	mat4 getWaterVP();
	void* getPtrWaterVP();

	mat4 getWaterRefrectMatrix();
	void* getPtrWaterfreMat();
	vec3 getWaterRefrectPosition();
	void* getPtrWaterfrePos();
	mat4 getWaterVP2();
	void* getPtrWaterVP2();

	void* getPtrRefleType();
	void* getPtrRefreType();

	ShaderObject *waterShader;
	ShaderObject *renderShader;
	void bindCamera(mat4 *pMatrix,vec3,vec3,vec3);
private:
	GLuint vao;
	vec3 eyeposition;
	vec3 forwardVector;
	vec3 upVector;
	mat4 pMatrix;
	GLuint dudvMap;
	GLuint fbo;
	GLuint fbo2;
	GLuint rbo;
	GLuint rbo2;
	GLuint fboTex;
	GLuint fboTex2;
	float moveFactor;
	float wave_speed;
	int reflectType;
	int refrectType;
};
