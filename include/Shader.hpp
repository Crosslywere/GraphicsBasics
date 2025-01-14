#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <thread>

// Function to open a file and place its contents in the store variable
static void extractTextFromFile(const std::string& file, std::string& store) {
	std::ifstream fileStream;
	fileStream.open(file);
	if (fileStream.is_open()) {
		std::stringstream stringStream;
		stringStream << fileStream.rdbuf();
		store = stringStream.str();
    }
    else {
        fprintf(stderr, "cannot open %s\n", file.c_str());
    }
}

// The shader class
class Shader {
public:
	~Shader() {
		glDeleteShader(m_ProgramObject);
	}
	// Loading from memory uses the parameters as the shader's source code
	static Shader CreateFromSource(const char* vertexSource, const char* fragmentSource) {
		return Shader(vertexSource, fragmentSource);
	}
	// Loading from file extracts the contents of the file to use as the shader's source code
	static Shader LoadFromFile(const char* vertexFile, const char* fragmentFile) {
		std::string vertexSource;
		std::string fragmentSource;
		std::thread vertThread{ [&]() { extractTextFromFile(vertexFile, vertexSource); } };
		std::thread fragThread{ [&]() { extractTextFromFile(fragmentFile, fragmentSource); } };
		vertThread.join();
		fragThread.join();
		return Shader(vertexSource.c_str(), fragmentSource.c_str());
	}
	// Sets the program to be active
	void UseProgram() const {
		glUseProgram(m_ProgramObject);
	}
	// Sets an integer uniform
	void SetInt(const char* name, int v) const {
		int loc = glGetUniformLocation(m_ProgramObject, name);
		if (loc >= 0) {
			glUniform1i(loc, v);
		}
	}
	// Sets a float uniform
	void SetFloat(const char* name, float v) const {
		int loc = glGetUniformLocation(m_ProgramObject, name);
		if (loc >= 0) {
			glUniform1f(loc, v);
		}
	}
    void SetFloat3(const char* name, const glm::vec3& v3) {
        int loc = glGetUniformLocation(m_ProgramObject, name);
        if (loc >= 0) {
            glUniform3fv(loc, 1, glm::value_ptr(v3));
        }
    }
	// Sets a mat4 uniform
	void SetMatrix4(const char* name, const glm::mat4& m) const {
		int loc = glGetUniformLocation(m_ProgramObject, name);
		if (loc >= 0) {
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
		}
	}
private:
	// Shader constructor
	Shader(const char* vertexSource, const char* fragmentSource) {
		m_ProgramObject = glCreateProgram();
		GLuint vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, &vertexSource, nullptr);
		glCompileShader(vert);
		if (CheckCompileStatus(vert)) {
			glAttachShader(m_ProgramObject, vert);
			glDeleteShader(vert);
		}
		else {
			return;
		}
		GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &fragmentSource, nullptr);
		glCompileShader(frag);
		if (CheckCompileStatus(frag)) {
			glAttachShader(m_ProgramObject, frag);
			glDeleteShader(frag);
		}
		else {
			return;
		}
		glLinkProgram(m_ProgramObject);
	}
	// Checks the compilation status of a shader and returns true if it was successfully compiled
	bool CheckCompileStatus(GLuint shader) {
		static constexpr int INFO_LOG_LENGTH = 500;
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[INFO_LOG_LENGTH];
			glGetShaderInfoLog(shader, INFO_LOG_LENGTH, nullptr, infoLog);
			fprintf(stderr, "%s\n", infoLog);
			return false;
		}
		return true;
	}
private:
	GLuint m_ProgramObject{};
};
