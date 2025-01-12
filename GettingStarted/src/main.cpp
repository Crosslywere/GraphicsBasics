#include <glad/glad.h>
#include <glfw/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION // needed to compile the stb image file
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
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
}

// The shader class
class Shader {
public:
	~Shader() {
		glDeleteShader(m_ProgramObject);
	}
	// Loading from memory uses the parameters as the shader's source code
	static Shader LoadFromMemory(const char* vertexSource, const char* fragmentSource) {
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

// Process keyboard inputs (for now only closes the application when escape is pressed)
static void processInputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main(int argc, char** argv) {
	// Initialize glfw library
	glfwInit();
	// Configure window properties before creation
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	// Create the window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Getting Started", nullptr, nullptr);
	if (window == nullptr) {
		exit(EXIT_FAILURE);
	}
	// Set the OpenGL context
	glfwMakeContextCurrent(window);
	// Set vsync to be on
	glfwSwapInterval(1);
	// Load opengl function pointers through glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		exit(EXIT_FAILURE);
	}

	// Specifying the vertices for the rectangle
	float vertices[] = {
		-.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-.5f, -.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	};

	// Specifying the order of vertices of the rectangle
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	// Transfering the rectangle data to the gpu
	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Transfering the vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Specifying the layout of the vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 5));
	glEnableVertexAttribArray(2);

	// Transfering the indices data
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Scoped so destructor is automatically called
	{
		// Loading the shader
		Shader shader = Shader::LoadFromFile("res/vert.glsl", "res/frag.glsl");
		// Loading the textures
		Texture texture1 = Texture::LoadFromFile("res/container.jpg");
		stbi_set_flip_vertically_on_load(true);
		Texture texture2 = Texture::LoadFromFile("res/awesomeface.png");
		// The projection matrix
		glm::mat4 proj = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f);
		// The model matrix
		glm::mat4 model{ 1.0f };
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// Processing keyboard inputs
			processInputs(window);
			// Clearing the color channel of the current framebuffer
			glClear(GL_COLOR_BUFFER_BIT);
			// Setting the shader to active
			shader.UseProgram();
			// Passing the textures to the shader
			texture1.Bind(1);
			shader.SetInt("uTexture1", 1);
			texture2.Bind(2);
			shader.SetInt("uTexture2", 2);
			// Setting other uniforms in the shader
			shader.SetFloat("uTime", glfwGetTime());
			// Setting the rotation of the model matrix using time
			model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
			// Setting the scale of the model matrix to x100
			model = glm::scale(model, glm::vec3(100.0f));
			// Setting the projection and model in the shader
			shader.SetMatrix4("uProj", proj);
			shader.SetMatrix4("uModel", model);
			// Binding the rectangle object
			glBindVertexArray(vao);
			// Drawing the rectangle using the currently active shader
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
			// Swapping the buffer beeing rendered
			glfwSwapBuffers(window);
		}
	}
	// Cleaning up the opengl objects
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	// Cleaning up glfw
	glfwDestroyWindow(window);
	glfwTerminate();
}
