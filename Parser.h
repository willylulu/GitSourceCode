#include "TextureLoader.h"

struct GROUP {
	int numVertices;
	unsigned int material_id;
};


class ObjParser : public TextureLoader {
	public:
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::vector<GROUP> *group;
		GLuint *vao;
		GLuint *texture;
		int shapeCount;
		mat4 normalizeMatrix;
		mat4 loadObj(char*);
	private:
		mat4 getNormalizeMatrix(std::vector<tinyobj::shape_t>);
};

class FbxParser : public TextureLoader {
	public:
		fbx_handles fbxHandles;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::vector<GROUP> *group;
		GLuint *vao;
		GLuint *texture;
		GLuint *positionBuffer;
		GLuint *normalBuffer;
		GLuint *textureCoordinateBuffer;
		GLuint *indexBuffer;
		int shapeCount;
		mat4 normalizeMatrix;
		mat4 loadFbx(char*);
	private:
		mat4 getNormalizeMatrix(std::vector<tinyobj::shape_t>);
};