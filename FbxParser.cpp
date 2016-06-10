#include "Parser.h"
#include <stdio.h>
mat4
FbxParser::loadFbx(char * url)
{
	std::string err;
	bool ret_fbx = LoadFbx(fbxHandles, shapes, materials, err, url);
	shapeCount = shapes.size();
	if (ret_fbx) {
		// For Each Material
		texture = new GLuint[materials.size()];
		for (int i = 0; i < materials.size(); i++) {
			int iter;
			for (iter = strlen(materials[i].diffuse_texname.c_str()) - 1; iter >= -1; iter--)
			{
				char dick = materials[i].diffuse_texname.c_str()[iter];
				if (dick ==  '\\'|| dick == '/')
				{
					break;
				}
			}
			texture[i] = loadTexture(materials[i].diffuse_texname.c_str()+iter+1);
		}
		// For Each Shape (or Mesh, Object)
		vao = new GLuint[shapes.size()];
		positionBuffer = new GLuint[shapes.size()];
		normalBuffer = new GLuint[shapes.size()];
		textureCoordinateBuffer = new GLuint[shapes.size()];
		indexBuffer = new GLuint[shapes.size()];
		group = new std::vector<GROUP>[shapes.size()];
		for (int i = 0; i < shapes.size(); i++)
		{
			glGenVertexArrays(1, &vao[i]);
			glBindVertexArray(vao[i]);
			glGenBuffers(1, &positionBuffer[i]);
			glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float), 0, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, shapes[i].mesh.positions.size() * sizeof(float), shapes[i].mesh.positions.data());
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			glGenBuffers(1, &normalBuffer[i]);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.normals.size() * sizeof(float), 0, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, shapes[i].mesh.normals.size() * sizeof(float), shapes[i].mesh.normals.data());
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glGenBuffers(1, &textureCoordinateBuffer[i]);
			glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.texcoords.size() * sizeof(float), 0, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, shapes[i].mesh.texcoords.size() * sizeof(float), shapes[i].mesh.texcoords.data());
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			glGenBuffers(1, &indexBuffer[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[i].mesh.indices.size() * sizeof(unsigned int), 0, GL_STATIC_DRAW);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, shapes[i].mesh.indices.size() * sizeof(unsigned int), shapes[i].mesh.indices.data());
			unsigned int tempMaterialIds = shapes[i].mesh.material_ids[0];
			for (int j = 0; j < shapes[i].mesh.material_ids.size(); j++) {
				if (shapes[i].mesh.material_ids[j] != tempMaterialIds) {
					group[i].push_back(GROUP{ j * 3, tempMaterialIds });
					tempMaterialIds = shapes[i].mesh.material_ids[j];
				}
			}
			group[i].push_back(GROUP{ (int)(shapes[i].mesh.material_ids.size() * 3), tempMaterialIds });
		}
	}
	normalizeMatrix = getNormalizeMatrix(shapes);
	return normalizeMatrix;
}
mat4 
FbxParser::getNormalizeMatrix(std::vector<tinyobj::shape_t> shapes) {
	float minVal[3];
	minVal[0] = minVal[1] = minVal[2] = 1e9;
	float maxVal[3];
	maxVal[0] = maxVal[1] = maxVal[2] = -1e9;

	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes[i].mesh.positions.size(); j += 3) {
			for (int k = 0; k < 3; k++) {
				if (shapes[i].mesh.positions.data()[j + k] > maxVal[k]) {
					maxVal[k] = shapes[i].mesh.positions.data()[j + k];
				}
				if (shapes[i].mesh.positions.data()[j + k] < minVal[k]) {
					minVal[k] = shapes[i].mesh.positions.data()[j + k];
				}
			}
		}
	}
	float translateVal[3];
	for (int i = 0; i < 3; i++) {
		translateVal[i] = ((maxVal[i] + minVal[i]) / 2) * -1;
	}
	float scaleVal = -1e9;
	for (int i = 0; i < 3; i++) {
		if (maxVal[i] - minVal[i] > scaleVal) {
			scaleVal = maxVal[i] - minVal[i];
		}
	}
	scaleVal = 1.0 / scaleVal;
	return glm::scale(mat4(1), vec3(scaleVal)) * glm::translate(mat4(1), vec3(translateVal[0], translateVal[1], translateVal[2]));
}