#include "Texture.h"
#include "GLSL.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



using namespace std;

int texUnit = 2;
std::map<std::string, std::shared_ptr<Texture>> uiTextures;

Texture::Texture() :
	filename(""),
	tid(0)
{
	
}

Texture::~Texture()
{
	
}

void Texture::init()
{
	// Load texture
	int w, h, ncomps;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(filename.c_str(), &w, &h, &ncomps, 0);
	if(!data) {
		cerr << filename << " not found" << endl;
	}
	if(ncomps != 3 && ncomps != 4) {
		cerr << filename << " must have 3 or 4 components (RGB) instead has " << ncomps << endl;
	}
	if((w & (w - 1)) != 0 || (h & (h - 1)) != 0) {
		cerr << filename << " must be a power of 2" << endl;
		std::exit(1);
	}
	width = w;
	height = h;

	// Generate a texture buffer object
	glGenTextures(1, &tid);
	// Bind the current texture to be the newly generated texture object
	glBindTexture(GL_TEXTURE_2D, tid);
	#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 
		float aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
		aniso = min(4.0f, aniso); // We only need at most 4x
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso); 
	#endif
	// Load the actual texture data
	// Base level is 0, number of channels is 3, and border is 0.
	if(ncomps == 3) {
  		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	} else if (ncomps == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	// Generate image pyramid
	glGenerateMipmap(GL_TEXTURE_2D);
	// Set texture wrap modes for the S and T directions
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Set filtering mode for magnification and minimification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	// Free image, since the data is now on the GPU
	stbi_image_free(data);
}

void Texture::setWrapModes(GLint wrapS, GLint wrapT)
{
	// Must be called after init()
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::bind(GLint handle)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tid);
	glUniform1i(handle, unit);
}

void Texture::unbind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}