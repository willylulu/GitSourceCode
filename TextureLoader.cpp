#include "TextureLoader.h"

GLuint
TextureLoader::loadTexture(const char *url) {
	printf("%s\n", url);
	GLuint texture;
	char fileType[3];
	if (strlen(url) > 3) {
		strncpy(fileType, url + strlen(url) - 3, 3);
	}
	int width, height;
	unsigned char *data;
	if (strcmp(fileType, "bmp") == 0) {
		FileHeader FH;
		InfoHeader IH;
		FILE * file;
		file = fopen(url, "rb");
		fread(&FH, sizeof(FileHeader), 1, file);
		fread(&IH, sizeof(InfoHeader), 1, file);
		if (file == NULL) {
			return 0;
		}
		width = IH.Width;
		height = IH.Height;
		data = new unsigned char[width * height * 3];
		fread(data, width * height * 3, 1, file);
		fclose(file);
		for (int i = 0; i < width * height; ++i) {
			int index = i * 3;
			unsigned char B, R;
			B = data[index];
			R = data[index + 2];
			data[index] = R;
			data[index + 2] = B;
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		delete[] data;
	}
	else{
		ILuint ilTexName;
		ilGenImages(1, &ilTexName);
		ilBindImage(ilTexName);
		if (ilLoadImage(url))
		{
			width = ilGetInteger(IL_IMAGE_WIDTH);
			height = ilGetInteger(IL_IMAGE_HEIGHT);
			data = new unsigned char[width * height * 4];
			ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
			delete[] data;
			ilDeleteImages(1, &ilTexName);
		}
	}
	return texture;
}