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

namespace phong {
    struct LightSource {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };
    struct DirectionalLight : public LightSource {
        glm::vec3 direction;
    };
    struct PointLight : public LightSource {
        glm::vec3 position;

        float constant = 1.f;
        float linear;
        float quadratic;
    };
    typedef struct SpotLight : public LightSource {
        glm::vec3 position;
        glm::vec3 direction;
        float innerCutoff;
        float outerCutoff;
        float constant = 1.f;
        float linear;
        float quadratic;
    } FlashLight;
}
// The shader class
class Shader {
public:
	~Shader() {
		glDeleteShader(m_ProgramObject);
	}
	/// <summary>Loading from memory uses the parameters as the shader's source code</summary>
    /// <param name="vertexSource">Source code for the vertex shader</param>
    /// <param name="fragmentSource">Source code for the fragment shader</param>
	static Shader CreateFromSource(const char* vertexSource, const char* fragmentSource) {
		return Shader(vertexSource, fragmentSource);
	}
	/// <summary>Loading from file extracts the contents of the file to use as the shader's source code</summary>
    /// <param name="vertexFile">File path to vertex shader source code file</param>
    /// <param name="fragmentFile">File path to fragment shader source code file</param>
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
    /// Sets variables for a struct in the shader:
    /// struct {
    ///     vec3 ambient;
    ///     vec3 diffuse;
    ///     vec3 specular;
    /// };
    /// Used like 'name.diffuse' where name is the string name parameter
    void SetLightColors(const std::string& name, const phong::LightSource& light) {
        SetFloat3((name + ".ambient").c_str(), light.ambient);
        SetFloat3((name + ".diffuse").c_str(), light.diffuse);
        SetFloat3((name + ".specular").c_str(), light.specular);
    }
    /// Sets variable for a struct in the shader:
    /// struct {
    ///     vec3 ambient;
    ///     vec3 diffuse;
    ///     vec3 specular;
    ///     vec3 direction;
    /// };
    /// Used like 'name.direction' where name is the string name parameter
    void SetLight(const std::string& name, const phong::DirectionalLight& light) {
        SetLightColors(name, light);
        SetFloat3((name + ".direction").c_str(), light.direction);
    }
    void SetLights(const std::string& name, const phong::DirectionalLight* lights, size_t size) {
        for (size_t i = 0; i < size; i++) {
            std::stringstream ss;
            ss << name << '[' << i << ']';
            SetLight(ss.str(), lights[i]);
        }
    }
    /// Sets variable for a struct in the shader:
    /// struct {
    ///     vec3 ambient;
    ///     vec3 diffuse;
    ///     vec3 specular;
    ///     vec3 position;
    ///     float constant;
    ///     float linear;
    ///     float quadratic;
    /// };
    /// Used like 'name.quadratic' where name is the string name parameter
    void SetLight(const std::string& name, const phong::PointLight& light) {
        SetLightColors(name, light);
        SetFloat3((name + ".position").c_str(), light.position);
        SetFloat((name + ".constant").c_str(), light.constant);
        SetFloat((name + ".linear").c_str(), light.linear);
        SetFloat((name + ".quadratic").c_str(), light.quadratic);
    }
    void SetLights(const std::string& name, const phong::PointLight* lights, size_t size) {
        for (size_t i = 0; i < size; i++) {
            std::stringstream ss;
            ss << name << '[' << i << ']';
            SetLight(ss.str(), lights[i]);
        }
    }
    /// Sets vrauble for a struct in the shader:
    /// struct {
    ///     vec3 ambient;
    ///     vec3 diffuse;
    ///     vec3 specular;
    ///     vec3 direction;
    ///     vec3 position;
    ///     float innerCutoff;
    ///     float outerCutoff;
    ///     float constant;
    ///     float linear;
    ///     float quadratic;
    /// };
    /// Used like 'name.cutoff' where name is the string name parameter
    void SetLight(const std::string& name, const phong::SpotLight& light) {
        SetLightColors(name, light);
        SetFloat3((name + ".direction").c_str(), light.direction);
        SetFloat3((name + ".position").c_str(), light.position);
        SetFloat((name + ".innerCutoff").c_str(), light.innerCutoff);
        SetFloat((name + ".outerCutoff").c_str(), light.outerCutoff);
        SetFloat((name + ".constant").c_str(), light.constant);
        SetFloat((name + ".linear").c_str(), light.linear);
        SetFloat((name + ".quadratic").c_str(), light.quadratic);
    }
    void SetLights(const std::string& name, const phong::FlashLight* lights, size_t size) {
        for (size_t i = 0; i < size; i++) {
            std::stringstream ss;
            ss << name << '[' << i << ']';
            SetLight(ss.str(), lights[i]);
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
