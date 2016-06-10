#include "skyBox.h"

char** SkyloadShaderSource(const char* file)
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

void SkyshaderLog(GLuint shader)
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

SkyBox::SkyBox(char** filenames)
{
	GLfloat vertices[] =
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	switchskybox = false;
	skyVertexs = vertices;
	skyProgram = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, SkyloadShaderSource("vertex.sky.glsl"), NULL);
	glShaderSource(fragmentShader, 1, SkyloadShaderSource("fragment.sky.glsl"), NULL);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	SkyshaderLog(vertexShader);
	SkyshaderLog(fragmentShader);
	glAttachShader(skyProgram, vertexShader);
	glAttachShader(skyProgram, fragmentShader);
	glLinkProgram(skyProgram);


	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	for (int i = 0; i < 6; i++)
	{
		FileHeader noiseTF; //BMP FileHeader 
		InfoHeader noiseTI; //BMP InfoHeader
		FILE *file = fopen(filenames[i], "rb");
		printf("%s\n", filenames[i]);
		fread(&noiseTF, sizeof(FileHeader), 1, file);
		fread(&noiseTI, sizeof(InfoHeader), 1, file);
		unsigned long size;
		unsigned char* data;
		size = noiseTI.Width * noiseTI.Height * 3;
		printf("%d\n", size);
		data = new unsigned char[size];
		fread(data, size * sizeof(char), 1, file);
		fclose(file);

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, noiseTI.Width, noiseTI.Height, 0, GL_BGR, GL_UNSIGNED_BYTE, data
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	GLuint bufferPosition;

	glGenBuffers(1, &bufferPosition);
	glBindBuffer(GL_ARRAY_BUFFER, bufferPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	genStarCubeMap();
}

GLuint SkyBox::getVAO()
{
	return skyboxVAO;
}

GLuint SkyBox::getTexture()
{
	return cubemapTexture;
}

GLuint SkyBox::getSkyProgram()
{
	return skyProgram;
}

GLfloat * SkyBox::getVertex()
{
	return skyVertexs;
}

void SkyBox::genStarCubeMap()
{
	char* skyBoxNames2[6] = { "pink_planet_pos_x.bmp","pink_planet_neg_x.bmp","pink_planet_pos_y.bmp","pink_planet_neg_y.bmp","pink_planet_neg_z.bmp","pink_planet_neg_z.bmp" };
	glGenTextures(1, &starCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, starCubeMap);
	for (int i = 0; i < 6; i++)
	{
		FileHeader noiseTF; //BMP FileHeader 
		InfoHeader noiseTI; //BMP InfoHeader
		FILE *file = fopen(skyBoxNames2[i], "rb");
		printf("%s\n", skyBoxNames2[i]);
		fread(&noiseTF, sizeof(FileHeader), 1, file);
		fread(&noiseTI, sizeof(InfoHeader), 1, file);
		unsigned long size;
		unsigned char* data;
		size = noiseTI.Width * noiseTI.Height * 3;
		printf("%d\n", size);
		data = new unsigned char[size];
		fread(data, size * sizeof(char), 1, file);
		fclose(file);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, noiseTI.Width, noiseTI.Height, 0, GL_BGR, GL_UNSIGNED_BYTE, data
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SkyBox::displaySkyBox(mat4*v, mat4*p, vec3*cp)
{
	bindVP_cP(v, p, cp);
	glDepthMask(GL_FALSE);
	glUseProgram(skyProgram);
	glActiveTexture(GL_TEXTURE0);
	GLuint finalSkyBox = switchskybox ? starCubeMap : cubemapTexture;
	glBindTexture(GL_TEXTURE_CUBE_MAP, finalSkyBox);
	mat4 tempV = *matrixV;
	mat4 tempP = *matrixP;
	vec3 _cp = *cameraPosition;
	glUniform3fv(glGetUniformLocation(skyProgram, "CameraPosition"), 1, value_ptr(_cp));
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "MVP"), 1, GL_FALSE, value_ptr(tempP*tempV));
	glUniform1i(glGetUniformLocation(skyProgram, "tex_cubemap"), 0);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthMask(GL_TRUE);
}

void SkyBox::bindVP_cP(mat4 *v, mat4 *p, vec3 *cp)
{
	matrixV = v;
	matrixP = p;
	cameraPosition = cp;
}

void SkyBox::switchSkyBox()
{
	switchskybox = switchskybox ? false : true;
}
