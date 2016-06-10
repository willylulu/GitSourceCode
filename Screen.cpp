#include "Screen.h"
#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

static const GLfloat window_positions[] =
{
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};

char** ScreenloadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

void ScreenfreeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

void ScreenshaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete errorLog;
	}
}

void loadTex(GLuint tex, char* filename)
{

}

Screen::Screen()
{
	unsigned char* temp;
	screenState = 0;
	colorFactor = 1;
	screenRbo = false;
	screenToon = false;
	screensmallmap = false;
	screenGroupTeam = false;
	for (int i = 0; i < 6; i++)isPressButton[i] = false;
	FileHeader noiseTF; //BMP FileHeader 
	InfoHeader noiseTI; //BMP InfoHeader
	unsigned long size;
	FILE *file = fopen("colorMap.bmp", "rb");
	fread(&noiseTF, sizeof(FileHeader), 1, file);
	fread(&noiseTI, sizeof(InfoHeader), 1, file);
	size = noiseTI.Width * noiseTI.Height * 3;
	printf("%d %d %d\n", noiseTI.Width, noiseTI.Height, size);
	temp = new unsigned char[size];
	fread(temp, size * sizeof(char), 1, file);
	fclose(file);

	glGenTextures(1, &noiseTex);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseTI.Width, noiseTI.Height, 0, GL_BGR, GL_UNSIGNED_BYTE, temp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	file = fopen("controlMap.bmp", "rb");
	fread(&noiseTF, sizeof(FileHeader), 1, file);
	fread(&noiseTI, sizeof(InfoHeader), 1, file);
	size = noiseTI.Width * noiseTI.Height * 3;
	printf("%d %d %d\n", noiseTI.Width, noiseTI.Height, size);
	temp = new unsigned char[size];
	fread(temp, size * sizeof(char), 1, file);
	fclose(file);

	glGenTextures(1, &controlTex);
	glBindTexture(GL_TEXTURE_2D, controlTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseTI.Width, noiseTI.Height, 0, GL_BGR, GL_UNSIGNED_BYTE, temp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	file = fopen("groupTeam.bmp", "rb");
	fread(&noiseTF, sizeof(FileHeader), 1, file);
	fread(&noiseTI, sizeof(InfoHeader), 1, file);
	size = noiseTI.Width * noiseTI.Height * 3;
	printf("%d %d %d\n", noiseTI.Width, noiseTI.Height, size);
	temp = new unsigned char[size];
	fread(temp, size * sizeof(char), 1, file);
	fclose(file);

	glGenTextures(1, &groupTeamTex);
	glBindTexture(GL_TEXTURE_2D, groupTeamTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseTI.Width, noiseTI.Height, 0, GL_BGR, GL_UNSIGNED_BYTE, temp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	screenProgram = glCreateProgram();
	GLuint vs2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs2, 1, ScreenloadShaderSource("vertex.screen.glsl"), NULL);
	glCompileShader(vs2);
	ScreenshaderLog(vs2);

	GLuint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs2, 1, ScreenloadShaderSource("fragment.screen.glsl"), NULL);
	glCompileShader(fs2);
	ScreenshaderLog(fs2);

	glAttachShader(screenProgram, vs2);
	glAttachShader(screenProgram, fs2);

	glLinkProgram(screenProgram);

	glGenVertexArrays(1, &screenVao);
	glBindVertexArray(screenVao);

	glGenBuffers(1, &screenBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, screenBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(window_positions), window_positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (const GLvoid*)(sizeof(GL_FLOAT) * 2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenFramebuffers(1, &FBO);
}

GLuint Screen::getScreenProgram()
{
	return screenProgram;
}

GLuint Screen::getScreenVal()
{
	return screenVao;
}

GLuint Screen::getScreenBuffer()
{
	return screenBuffer;
}

GLuint Screen::getFBO()
{
	return FBO;
}

GLuint Screen::getDepthRBO()
{
	return depthRBO;
}

GLuint Screen::getFBODataTexture()
{
	return FBODataTexture;
}

GLuint Screen::getNoiseTex()
{
	return noiseTex;
}

void Screen::reShape(float width, float height)
{
	glDeleteRenderbuffers(1, &depthRBO);
	glDeleteTextures(1, &FBODataTexture);
	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

	glGenTextures(1, &FBODataTexture);
	glBindTexture(GL_TEXTURE_2D, FBODataTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBODataTexture, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	screenHeight = height;
	screenWidth = width;
}

void Screen::displayScreen()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, groupTeamTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, controlTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBODataTexture);
	glBindVertexArray(screenVao);
	glUseProgram(screenProgram);
	glUniform1i(glGetUniformLocation(screenProgram, "mapFlag"), screenState);
	float t_mainPosition[2] = { *mainPosition[0],*mainPosition[1] };
	glUniform2fv(glGetUniformLocation(screenProgram, "mainPosition"), 1, t_mainPosition);
	glUniform2fv(glGetUniformLocation(screenProgram, "mapSize"), 1, mapSize);
	glUniform1f(glGetUniformLocation(screenProgram, "buttonX"), buttonX);
	glUniform1f(glGetUniformLocation(screenProgram, "screenWidth"), screenWidth);
	glUniform1f(glGetUniformLocation(screenProgram, "screenHeight"), screenHeight);
	glUniform1f(glGetUniformLocation(screenProgram, "colorFactor"), colorFactor);
	glUniform1i(glGetUniformLocation(screenProgram, "switchRbo"), screenRbo ? 1 : 0);
	glUniform1i(glGetUniformLocation(screenProgram, "switchToon"), screenToon ? 1 : 0);
	glUniform1i(glGetUniformLocation(screenProgram, "switchsmallmap"), screensmallmap ? 1 : 0);
	glUniform1i(glGetUniformLocation(screenProgram, "screenGroupTeam"), screenGroupTeam ? 1 : 0);
	for (int i = 0; i < 6; i++)
	{
		std::string temp;
		std::string temp1("isPressButton[");
		std::string temp2 = std::to_string(i);
		std::string temp3("]");
		temp = temp1 + temp2 + temp3;
		glUniform1i(glGetUniformLocation(screenProgram, temp.c_str()), isPressButton[i] ? 1 : 0);
		temp1 = "buttonListY[";
		temp2 = std::to_string(i);
		temp3 = "]";
		temp = temp1 + temp2 + temp3;
		glUniform1f(glGetUniformLocation(screenProgram, temp.c_str()), buttonListY[i]);
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Screen::displayScreen(GLuint fbo)
{

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo);
	glBindVertexArray(screenVao);
	glUseProgram(screenProgram);
	glUniform1i(glGetUniformLocation(screenProgram, "mapFlag"), screenState);
	float t_mainPosition[2] = { *mainPosition[0],*mainPosition[1] };
	glUniform2fv(glGetUniformLocation(screenProgram, "mainPosition"), 1, t_mainPosition);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Screen::setScreenState(int flag)
{
	screenState = flag;
}

void Screen::bindMainPosition(FirstMan* firstMan)
{
	mainPosition[0] = &firstMan->getPtrPos()->x;
	mainPosition[1] = &firstMan->getPtrPos()->z;
}

void Screen::bindMapSize(float w, float h)
{
	mapSize[0] = w;
	mapSize[1] = h;
}

void Screen::bindButton(std::vector<float> r, float x)
{
	this->buttonListY = r;
	this->buttonX = x;
}

void Screen::startFbo()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glDrawBuffer(GL_DRAW_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::endFbo()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::switchButton(int i)
{
	isPressButton[5 - i] = isPressButton[5 - i] ? false : true;
}

void Screen::addColorFactor()
{
	colorFactor += 0.1;
}

void Screen::minusColorFactor()
{
	colorFactor -= 0.1;
}

void Screen::switchRbo()
{
	if (isPressButton[3])
	{
		isPressButton[3] = false;
		screenToon = false;
	}
	screenRbo = screenRbo ? false : true;
}

void Screen::switchToon()
{
	if (isPressButton[1])
	{
		isPressButton[1] = false;
		screenRbo = false;
	}
	screenToon = screenToon ? false : true;
}

void Screen::switchsmallmap()
{
	screensmallmap = screensmallmap ? false : true;
}

void Screen::switchGroupTeam()
{
	screenGroupTeam = screenGroupTeam ? false : true;
}
