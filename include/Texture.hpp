#pragma once

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstdio>

// The texture class
class Texture {
public:
	~Texture() {
		glDeleteTextures(1, &m_TextureObject);
	}
	// Loads a texture from a file using nothings' stb image library
	static Texture LoadFromFile(const char* filepath) {
		return Texture(filepath);
	}
	// Sets the texture as active using the optional index (defaults to 0)
	void Bind(unsigned int index = 0) const {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, m_TextureObject);
	}
private:
	// Texture file constructor
	Texture(const char* filepath) {
		int channels;
		unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &channels, 0);
		if (data == nullptr) {
			fprintf(stderr, "%s\n", stbi_failure_reason());
			return;
		}
		glGenTextures(1, &m_TextureObject);
		glBindTexture(GL_TEXTURE_2D, m_TextureObject);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
private:
	GLuint m_TextureObject{};
	int m_Width{}, m_Height{};
};
