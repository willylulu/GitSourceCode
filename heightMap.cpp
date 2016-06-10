#include "heightMap.h"

char** HeightloadShaderSource(const char* file)
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

void HeightfreeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

void HeightshaderLog(GLuint shader)
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

HeightMap::HeightMap(char * filename,float value,float w,float h)
{
	FileHeader noiseTF; //BMP FileHeader 
	InfoHeader noiseTI; //BMP InfoHeader
	printf("%s\n", filename);
	unsigned long size;
	FILE *file = fopen(filename, "rb");
	fread(&noiseTF, sizeof(FileHeader), 1, file);
	fread(&noiseTI, sizeof(InfoHeader), 1, file);
	size = noiseTI.Width * noiseTI.Height * 3;
	height = noiseTI.Height;
	width = noiseTI.Width;
	data = new unsigned char[size];
	fread(data, size * sizeof(char), 1, file);
	fclose(file);

	gravityValue = value;
	mapWidth = w;
	mapHeigh = h;

	Look = lookAt(vec3(0,500,0),vec3(0,0,0),vec3(0,0,-1));
	Othro = ortho(500,-500,500,-500,1,10000);

	program = glCreateProgram();
	GLuint vs2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs2, 1, HeightloadShaderSource("vertex.heightMap.glsl"), NULL);
	glCompileShader(vs2);
	HeightshaderLog(vs2);

	GLuint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs2, 1, HeightloadShaderSource("fragment.heightMap.glsl"), NULL);
	glCompileShader(fs2);
	HeightshaderLog(fs2);

	glAttachShader(program, vs2);
	glAttachShader(program, fs2);

	glLinkProgram(program);

	glGenFramebuffers(1, &Fbo);
}

float HeightMap::getHeight(float x, float y)
{
	vec2 t = vec2((x + mapWidth / 2) / mapWidth * width, (y + mapHeigh / 2) / mapHeigh* height);
	int ax = floor(t.x);
	int ay = floor(t.y);
	float y0 = data[(ay*width + ax) * 3];
	vec2 x0 = vec2(floor(t.x), floor(t.y));
	int bx = ceil(t.x);
	int by = ceil(t.y);
	float y1 = data[(by*width + bx) * 3];
	vec2 x1 = vec2(ceil(t.x), ceil(t.y));
	vec2 _x = vec2(t.x, t.y);
	float finalY = y0 + distance(_x, x0)*(y1 - y0) / distance(x1, x0);

	return ((finalY / 255) - 0.5) * 500 + 5;
}

void HeightMap::gravity(FbxEntity & target,float time)
{
	if (target.position.y <= getHeight(target.position.x, target.position.z))
	{
		target.position.y = getHeight(target.position.x, target.position.z);
		target.speed.y = 0;
	}
	else
	{
		target.onAir = true;
		target.position.y += target.speed.y;
		target.speed.y -= gravityValue*time;
	}
}

void HeightMap::gravity(StaticObject & target, float time)
{
	if (target.position.y <= getHeight(target.position.x, target.position.z))
	{
		target.onAir = true;
		target.position.y = getHeight(target.position.x, target.position.z);
		target.speed.y = 0;
	}
	else
	{
		target.position.y += target.speed.y;
		target.speed.y -= gravityValue*time;
	}
}

void HeightMap::gravity(FirstMan &firstMan,float time)
{
	if (firstMan.getPosition().y <= getHeight(firstMan.getPosition().x, firstMan.getPosition().z))
	{
		firstMan.setPosition(vec3(firstMan.getPosition().x, getHeight(firstMan.getPosition().x, firstMan.getPosition().z), firstMan.getPosition().z));
		firstMan.setSpeed(vec3(firstMan.getSpeed().x,0 , firstMan.getSpeed().z));
	}
	else
	{
		firstMan.setPosition(vec3(firstMan.getPosition().x, firstMan.getPosition().y+ firstMan.getSpeed().y, firstMan.getPosition().z));
		firstMan.setSpeed(vec3(firstMan.getSpeed().x, firstMan.getSpeed().y- gravityValue*time, firstMan.getSpeed().z));
	}
}

GLuint HeightMap::getProgram()
{
	return program;
}

GLuint HeightMap::getTexFbo()
{
	return TexFbo;
}

mat4 HeightMap::getVMatrix()
{
	return lookAt(vec3(0, 500, 0), vec3(0, 0, 0), vec3(0, 0, 1));
}

mat4 HeightMap::getVMatrix(vec3 *eyeposition, vec3 *forwardVector,vec3 *upVector)
{
	return lookAt(vec3(0, 500, 0), vec3(0, 0,0),vec3(0,0,1));
}

mat4 HeightMap::getPMatrix()
{
	return ortho(500.0f, -500.0f, 500.0f, -500.0f, 0.0f, 10000.0f);
}

mat4 HeightMap::getVPMatrix()
{
	return Othro*Look;
}

void HeightMap::startFbo()
{
	glBindFramebuffer(GL_FRAMEBUFFER, Fbo);
	glDrawBuffer(GL_DRAW_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void HeightMap::endFbo()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void HeightMap::reshape(float width, float height)
{
	width = 1000;
	height = 1000;
	//glDeleteRenderbuffers(1, &Rbo);
	glDeleteTextures(1, &TexFbo);
	glGenRenderbuffers(1, &Rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

	glGenTextures(1, &TexFbo);
	glBindTexture(GL_TEXTURE_2D, TexFbo);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Fbo);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Rbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TexFbo, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
