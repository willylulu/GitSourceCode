#include <glew.h>
#include <freeglut.h>
#include <stdio.h>
#include <vector>

class ShaderObject {
	public:
		GLuint program;
		GLuint uniformModelMatrix;
		std::vector<GLuint> uniform;
		std::vector<char*> uniformReference;
		ShaderObject(char*,char*);
		void initialUniform(std::vector<char*>);
	private:
		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint geometryShader;
		char** loadShaderSource(const char*);
		void freeShaderSource(char**);
		void shaderLog(GLuint);
};