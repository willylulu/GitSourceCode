#include <glew.h>
#include <freeglut.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "firstman.h"
using namespace glm;

class WingJean {
public:
	WingJean(FirstMan *firstman, bool* flying, bool* dragLock);
	void getWingJean();
	void startWingJean();
	void endWingJean();
private:
	float* data;
	int target;
	bool* flying;
	bool* dragLock;
	vec3* eyePosition;
	vec3* eyeDirection;
	vec3 originPos;
	vec3 originDir;
};