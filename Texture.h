#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GLFW/glfw3.h>

#include "stb_image.h"
class Texture
{
	unsigned char data[4] = { 255, 255, 255, 255 };
public : unsigned int emptyTex;
public:
	
	Texture() {
		/*
		Generates empty texture for object
		*/
		this->emptyTex = generateTextureWrapper();
		if (!bindTexture(emptyTex, 2))
			std::cerr << "DEFAULT TEXTURE NOT CREATED, ERRORS WILL OCCUR!";
		setParameters();
		generateTexture(data, 1, 1);
	}; //generates empty texture

	void clean(unsigned int* textureID) {
		glDeleteTextures(1, textureID);
	}
protected:
	unsigned int newTexture(std::string& textureName) {
		unsigned int texID = generateTextureWrapper();
		if(!bindTexture(texID,2))
			std::cerr << "DEFAULT TEXTURE NOT CREATED, ERRORS WILL OCCUR!";
		setParameters();
		unsigned char* data = loadImage(textureName);
		freeImage(data);
		return texID;
	};//loads new texture to image
private:
	unsigned int generateTextureWrapper() {
		unsigned int texture;
		glGenTextures(1, &texture);
		return texture;
	};
	bool bindTexture(unsigned int& texture, char type) {
		if (type == 2)
			glBindTexture(GL_TEXTURE_2D, texture);
		else if (type == 1)
			glBindTexture(GL_TEXTURE_1D, texture);
		else if (type == 3)
			glBindTexture(GL_TEXTURE_3D, texture);
		else{
			std::cout << "\n Texture type "<< type << "D unknown. Types are: 1D, 2D, 3D" << std::endl;
			return false;
		}
		std::cout << "\n Successfully bound texture " << texture << std::endl;
		return true;
	};
	void setParameters() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//filter for when camera far away
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//filter for when camera close
	};
	unsigned char* loadImage(std::string& textureName) {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			std::cout << "Sucessfully loaded image";
			generateTexture(data, width, height);
		}
		else {
			std::cout << "\n Failed to load texture" << std::endl;
			return nullptr;
		}
		return data;
	};
	bool generateTexture(unsigned char* data, int width, int height) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return true;
	};
	void freeImage(unsigned char* data) {
		stbi_image_free(data);
	}

};

#endif
