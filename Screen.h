#include <glew.h>
#include <freeglut.h>
#include <vector>
#include "firstman.h"
class Screen {
public:
	Screen();
	GLuint getScreenProgram();
	GLuint getScreenVal();
	GLuint getScreenBuffer();
	GLuint getFBO();
	GLuint getDepthRBO();
	GLuint getFBODataTexture();
	GLuint getNoiseTex();
	void reShape(float width, float height);
	void displayScreen();
	void displayScreen(GLuint fbo);
	void setScreenState(int);
	void bindMainPosition(FirstMan*);
	void bindMapSize(float, float);
	void bindButton(std::vector<float> r, float x);
	void startFbo();
	void endFbo();
	void switchButton(int);
	void addColorFactor();
	void minusColorFactor();
	void switchRbo();
	void switchToon();
	void switchsmallmap();
	void switchGroupTeam();
private:
	GLuint screenProgram;
	GLuint screenVao;
	GLuint screenBuffer;
	GLuint FBO;
	GLuint depthRBO;
	GLuint FBODataTexture;
	GLuint noiseTex;
	GLuint controlTex;
	GLuint groupTeamTex;
	int screenState;
	float* mainPosition[2];
	float mapSize[2];
	bool isPressButton[6];
	std::vector<float> buttonListY;
	float buttonX;
	float screenWidth;
	float screenHeight;
	float colorFactor;
	bool screenRbo;
	bool screenToon;
	bool screensmallmap;
	bool screenGroupTeam;
};
