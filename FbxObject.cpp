#include "Render.h"

FbxEntity::FbxEntity(char* url, vec3 position, vec3 rotation, vec3 scale) {
	mat4 t_normalizeMatrix = loadFbx(url);
	this->onAir = false;
	this->speed = vec3(0);
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	calculateModelMatrix();
}

void
FbxEntity::save_animation(std::vector<float> time_list) {
	int count = time_list.size() / 2;
	for (int i = 0; i <count; i++) {
		time_lapse temp;
		temp.end = time_list[time_list.size() - 1];
		time_list.pop_back();
		temp.start = time_list[time_list.size() - 1];
		time_list.pop_back();
		animation_lapse.push_back(temp);
	}

}

void
FbxEntity::do_animation(int animation_stage) {


	std::vector<tinyobj::shape_t> new_shapes;
	GetFbxAnimation(fbxHandles, new_shapes, time_animate);

	for (int i = 0; i < new_shapes.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, new_shapes[i].mesh.positions.size() * sizeof(float), 0, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, new_shapes[i].mesh.positions.size() * sizeof(float), new_shapes[i].mesh.positions.data());
	}
	if (animate_stop == 0)
		time_animate = time_animate + 0.005 / animation_lapse.size();
	if (time_animate >= animation_lapse[animation_stage].end) {
		time_animate = animation_lapse[animation_stage].start;
	}
}

void
FbxEntity::render(ShaderObject *shader, std::vector<UniformData> dataList) {
	glUseProgram(shader->program);
	calculateModelMatrix();
	setUniformData(dataList, shader);
	for (int i = 0; i < shapeCount; i++) {
		unsigned int offset = 0;
		for (int j = 0; j < group[i].size(); j++) {
			glBindVertexArray(vao[i]);
			glBindTexture(GL_TEXTURE_2D, texture[group[i][j].material_id]);
			if (j != 0)
				glDrawElements(GL_TRIANGLES, group[i][j].numVertices - group[i][j - 1].numVertices, GL_UNSIGNED_INT, (GLvoid*)(offset * sizeof(GLfloat)));
			else
				glDrawElements(GL_TRIANGLES, group[i][j].numVertices, GL_UNSIGNED_INT, (GLvoid*)(offset * sizeof(GLfloat)));
			offset += group[i][j].numVertices;
		}
	}

}


void
FbxEntity::assignUniformData(char *variable, int amount, int*data, ShaderObject *shader) {
	int index = std::find(shader->uniformReference.begin(), shader->uniformReference.end(), variable) - shader->uniformReference.begin();
	if (amount == 1) {
		glUniform1iv(shader->uniform[index], 1, data);
	}
	else if (amount == 2) {
		glUniform2iv(shader->uniform[index], 1, data);
	}
	else if (amount == 3) {
		glUniform3iv(shader->uniform[index], 1, data);
	}
	else if (amount == 4) {
		glUniform4iv(shader->uniform[index], 1, data);
	}
}

void
FbxEntity::assignUniformData(char *variable, int amount, float* data, ShaderObject *shader) {
	int index = std::find(shader->uniformReference.begin(), shader->uniformReference.end(), variable) - shader->uniformReference.begin();
	if (amount == 1) {
		glUniform1fv(shader->uniform[index], 1, data);
	}
	else if (amount == 2) {
		glUniform2fv(shader->uniform[index], 1, data);
	}
	else if (amount == 3) {
		glUniform3fv(shader->uniform[index], 1, data);
	}
	else if (amount == 4) {
		glUniform4fv(shader->uniform[index], 1, data);
	}
}

void
FbxEntity::assignUniformData(char *variable, vec3 &data, ShaderObject *shader) {
	int index = std::find(shader->uniformReference.begin(), shader->uniformReference.end(), variable) - shader->uniformReference.begin();
	glUniform3fv(uniform[index], 1, value_ptr(data));
}

void
FbxEntity::assignUniformData(char *variable, vec4 &data, ShaderObject *shader) {
	int index = std::find(shader->uniformReference.begin(), shader->uniformReference.end(), variable) - shader->uniformReference.begin();
	glUniform4fv(shader->uniform[index], 1, value_ptr(data));
}

void
FbxEntity::assignUniformData(char *variable, mat4 &data, ShaderObject *shader) {
	int index = std::find(shader->uniformReference.begin(), shader->uniformReference.end(), variable) - shader->uniformReference.begin();
	glUniformMatrix4fv(shader->uniform[index], 1, GL_FALSE, value_ptr(data));
}

mat4
FbxEntity::getModelMatrix()
{
	return mMatrix;
}

void
FbxEntity::setUniformData(std::vector<UniformData> dataList, ShaderObject *shader) {
	for (int i = 0; i < dataList.size(); i++) {
		if (dataList[i].type == "int") {
			assignUniformData(dataList[i].variable, dataList[i].amount, (int*)dataList[i].data, shader);
		}
		else if (dataList[i].type == "float") {
			assignUniformData(dataList[i].variable, dataList[i].amount, (float*)dataList[i].data, shader);
		}
		else if (dataList[i].type == "vec3") {
			vec3 v = glm::make_vec3((float*)dataList[i].data);
			assignUniformData(dataList[i].variable, v, shader);
		}
		else if (dataList[i].type == "vec4") {
			vec4 v = glm::make_vec4((float*)dataList[i].data);
			assignUniformData(dataList[i].variable, v, shader);
		}
		else if (dataList[i].type == "mat4") {
			mat4 m = glm::make_mat4((float*)dataList[i].data);
			assignUniformData(dataList[i].variable, m, shader);
		}
	}
	glUniformMatrix4fv(shader->uniformModelMatrix, 1, GL_FALSE, value_ptr(mMatrix));
}

void
FbxEntity::calculateModelMatrix() {
	mMatrix = glm::translate(mat4(1), position)
		* glm::rotate(mat4(1), rotation.x, vec3(1, 0, 0))
		* glm::rotate(mat4(1), rotation.y, vec3(0, 1, 0))
		* glm::rotate(mat4(1), rotation.z, vec3(0, 0, 1))
		* glm::scale(mat4(1), scale)
		* normalizeMatrix;
}
