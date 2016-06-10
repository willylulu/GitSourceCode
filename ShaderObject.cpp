#include "ShaderObject.h"

ShaderObject::ShaderObject(char *vertexfile, char *fragmentfile)
{
	printf("%s %s\n", vertexfile, fragmentfile);
	program = glCreateProgram();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource(vertexfile);
	char** fragmentShaderSource = loadShaderSource(fragmentfile);
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	shaderLog(vertexShader);
	shaderLog(fragmentShader);
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
}

char** 
ShaderObject::loadShaderSource(const char* file){
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

void 
ShaderObject::freeShaderSource(char** srcp){
	delete srcp[0];
	delete srcp;
}

void 
ShaderObject::shaderLog(GLuint shader)
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

void
ShaderObject::initialUniform(std::vector<char*> list) {
	uniformReference = list;
	for (int i = 0; i < list.size(); i++) {
		uniform.push_back(glGetUniformLocation(program, list[i]));
	}
	uniformModelMatrix = glGetUniformLocation(program, "mMatrix");
}