#include "render.h"

Water::Water(float speed,char* vs2,char* fs2)
{
	reflectType = 0;
	refrectType = 1;
	this->wave_speed = speed;
	unsigned char* data;
	FileHeader noiseTF; //BMP FileHeader 
	InfoHeader noiseTI; //BMP InfoHeader
	unsigned long size;
	FILE *file = fopen("waterDUDV.bmp", "rb");
	fread(&noiseTF, sizeof(FileHeader), 1, file);
	fread(&noiseTI, sizeof(InfoHeader), 1, file);
	size = noiseTI.Width * noiseTI.Height * 3;
	data = new unsigned char[size];
	fread(data, size * sizeof(char), 1, file);
	fclose(file);

	glGenTextures(1, &dudvMap);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseTI.Width, noiseTI.Height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	waterShader = new ShaderObject("vertex.water.glsl", "fragment.water.glsl");

	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	GLuint bufferPosition;

	glGenBuffers(1, &bufferPosition);
	glBindBuffer(GL_ARRAY_BUFFER, bufferPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertexs), waterVertexs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	glGenFramebuffers(1,&fbo);
	glGenFramebuffers(1, &fbo2);

	this->genRenderProgram(vs2,fs2);

	std::vector<char*> uniformList;
	uniformList.push_back("mMatrix");
	uniformList.push_back("vpMatrix");
	uniformList.push_back("toggler");
	renderShader->initialUniform(uniformList);
}

void Water::displayWater(mat4 &um4mvp,mat4 &um4v,float time)
{
	setChange(time);
	glUseProgram(waterShader->program);
	glUniformMatrix4fv(glGetUniformLocation(waterShader->program, "um4mvp"), 1, GL_FALSE, value_ptr(um4mvp));
	glUniformMatrix4fv(glGetUniformLocation(waterShader->program, "um4v"), 1, GL_FALSE, value_ptr(um4v));
	glUniform3fv(glGetUniformLocation(waterShader->program, "eyeposition"), 1,value_ptr(eyeposition));
	glUniform1f(glGetUniformLocation(waterShader->program, "moveFactor"),moveFactor);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, fboTex2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fboTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 30);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindVertexArray(0);
}

void Water::reshpae(float width, float height)
{
	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &fboTex);
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

	glGenTextures(1, &fboTex);
	glBindTexture(GL_TEXTURE_2D, fboTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glDeleteRenderbuffers(1, &rbo2);
	glDeleteTextures(1, &fboTex2);
	glGenRenderbuffers(1, &rbo2);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

	glGenTextures(1, &fboTex2);
	glBindTexture(GL_TEXTURE_2D, fboTex2);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo2);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo2);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex2, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Water::startFbo()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_DRAW_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
}

void Water::endFbo()
{
	glDisable(GL_CLIP_DISTANCE0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint Water::getFboTex()
{
	return fboTex;
}

void Water::startFbo2()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo2);
	glDrawBuffer(GL_DRAW_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
}

void Water::endFbo2()
{
	glDisable(GL_CLIP_DISTANCE0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint Water::getFboTex2()
{
	return fboTex2;
}

void Water::genRenderProgram(char * vs2, char * fs2)
{
	renderShader = new ShaderObject(vs2,fs2);
}

GLuint Water::getRenderProgram()
{
	return renderShader->program;
}

void Water::setChange(float time)
{
	moveFactor += wave_speed*time;
	moveFactor -= floor(moveFactor);
}

mat4 Water::getWaterReflectMatrix()
{
	return lookAt(vec3(eyeposition.x, -eyeposition.y + 50, eyeposition.z), vec3(eyeposition.x, -eyeposition.y + 50, eyeposition.z) + vec3(forwardVector.x, -forwardVector.y, forwardVector.z), upVector);
}

mat4 Water::getWaterRefrectMatrix()
{
	return lookAt(vec3(eyeposition.x, eyeposition.y, eyeposition.z), vec3(eyeposition.x, eyeposition.y, eyeposition.z) + vec3(forwardVector.x, forwardVector.y, forwardVector.z), upVector);
}

void * Water::getPtrWaterRefMat()
{
	mat4 temp = getWaterReflectMatrix();
	return value_ptr(temp);
}

void * Water::getPtrWaterfreMat()
{
	mat4 temp = getWaterRefrectMatrix();
	return value_ptr(temp);
}

vec3 Water::getWaterReflectPosition()
{
	return vec3(eyeposition.x, -eyeposition.y + 50, eyeposition.z);
}

vec3 Water::getWaterRefrectPosition()
{
	return vec3(eyeposition.x, eyeposition.y, eyeposition.z);
}

void * Water::getPtrWaterRefPos()
{
	vec3 temp = getWaterReflectPosition();
	return value_ptr(temp);
}

void * Water::getPtrWaterfrePos()
{
	vec3 temp = getWaterRefrectPosition();
	return value_ptr(temp);
}

mat4 Water::getWaterVP()
{
	return pMatrix * getWaterReflectMatrix();
}

void * Water::getPtrWaterVP()
{
	mat4 temp = getWaterVP();
	return value_ptr(temp);
}

mat4 Water::getWaterVP2()
{
	return pMatrix * getWaterRefrectMatrix();
}

void * Water::getPtrWaterVP2()
{
	mat4 temp = getWaterVP2();
	return value_ptr(temp);
}

void * Water::getPtrRefleType()
{
	return &reflectType;
}

void * Water::getPtrRefreType()
{
	return &refrectType;
}

void Water::bindCamera(mat4 * pMatrix, vec3 p, vec3 f, vec3 u)
{
	eyeposition = p;
	forwardVector = f;
	upVector = u;
	this->pMatrix = *pMatrix;
}
