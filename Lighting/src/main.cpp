#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <Shader.hpp>
#include <Texture.hpp>
#include <Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <thread>

constexpr auto CAMERA_SPEED = 2.5f;

static struct UserPointer {
    Camera* cameraPtr;
    float deltaTime{ 0.f };
} userPtr;

// Process keyboard inputs
static void processInputs(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // If the mouse button is released set its mode from disabled to normal
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Movement controls (fly camera)
    UserPointer* userPtr = (UserPointer*)glfwGetWindowUserPointer(window);
    if (userPtr != nullptr && userPtr->cameraPtr != nullptr) {
        glm::vec2 moveDir{ 0.f, 0.f };
        float up = 0.f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            moveDir.y += 1.f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            moveDir.y -= 1.f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            moveDir.x -= 1.f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            moveDir.x += 1.f;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            up -= 1.f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            up += 1.f;
        }
        Camera& camera = *userPtr->cameraPtr;
        // Fixing movement
        if (glm::length(moveDir) != 0) {
            moveDir = glm::normalize(moveDir);
            camera.SetPosition(camera.GetPosition() + camera.GetFront() * moveDir.y * CAMERA_SPEED * userPtr->deltaTime);
            camera.SetPosition(camera.GetPosition() + camera.GetRight() * moveDir.x * CAMERA_SPEED * userPtr->deltaTime);
        }
        camera.SetPosition(camera.GetPosition() + Camera::WORLD_UP * up * userPtr->deltaTime);
    }
}

static void cursorPosFn(GLFWwindow* window, double xpos, double ypos) {
    UserPointer* userPtr = (UserPointer*)glfwGetWindowUserPointer(window);
    if (userPtr != nullptr && userPtr->cameraPtr != nullptr) {
        static bool justPressed = true;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            static double prevX = 0.;
            static double prevY = 0.;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (justPressed) {
                prevX = xpos;
                prevY = ypos;
                justPressed = false;
            }
            float xOffset = prevX - xpos;
            prevX = xpos;
            float yOffset = prevY - ypos;
            prevY = ypos;
            Camera& camera = *userPtr->cameraPtr;
            camera.SetPitch(camera.GetPitch() + yOffset * userPtr->deltaTime * CAMERA_SPEED);
            camera.SetYaw(camera.GetYaw() + xOffset * userPtr->deltaTime * CAMERA_SPEED);
        }
        else {
            justPressed = true;
        }
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting", nullptr, nullptr);
    if (window == nullptr) {
        exit(EXIT_FAILURE);
    }
    // Set the OpenGL context
    glfwMakeContextCurrent(window);
    // Setting the window's userpointer
    glfwSetWindowUserPointer(window, &userPtr);
    glfwSetCursorPosCallback(window, cursorPosFn);
    // Set vsync to be on
    glfwSwapInterval(1);
    // Load opengl function pointers through glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        exit(EXIT_FAILURE);
    }

    // Specifying the vertices for the cube
    float vertices[] = {
        // FRONT
        -.5f, 0.5f, -.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.f,
        0.5f, 0.5f, -.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.f,
        0.5f, -.5f, -.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.f,
        -.5f, -.5f, -.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.f,
        // BACK
        -.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -.5f, -.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        // TOP
        -.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -.5f, 0.5f, -.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // BOTTOM
        -.5f, -.5f, -.5f, 0.0f, 1.0f, 0.0f, -1.f, 0.0f,
        0.5f, -.5f, -.5f, 1.0f, 1.0f, 0.0f, -1.f, 0.0f,
        0.5f, -.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.f, 0.0f,
        -.5f, -.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.f, 0.0f,
        // LEFT
        -.5f, 0.5f, -.5f, 0.0f, 1.0f, -1.f, 0.0f, 0.0f,
        -.5f, 0.5f, 0.5f, 1.0f, 1.0f, -1.f, 0.0f, 0.0f,
        -.5f, -.5f, 0.5f, 1.0f, 0.0f, -1.f, 0.0f, 0.0f,
        -.5f, -.5f, -.5f, 0.0f, 0.0f, -1.f, 0.0f, 0.0f,
        // RIGHT
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -.5f, -.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    };

    // Specifying the order of vertices of the cube
    unsigned int indices[] = {
        //Front Face
         0, 1, 2,
         2, 3, 0,
        // Back Face
         4, 5, 6,
         6, 7, 4,
        // Top Face
         8, 9,10,
        10,11, 8,
        // Bottom Face
        12,13,14,
        14,15,12,
        // Left Face
        16,17,18,
        18,19,16,
        // Right Face
        20,21,22,
        22,23,20,
    };

    // Transfering the cube data to the gpu
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

    // Enabling depth testing
    glEnable(GL_DEPTH_TEST);
    glClearColor(.1f, .1f, .1f, 1.f);

    // Scoped so destructor is automatically called
    {
        // Loading the shaders
        // Shader containerShader = Shader::LoadFromFile("res/vert.glsl", "res/basic_phong_frag.glsl");
        Shader containerShader = Shader::LoadFromFile("res/vert.glsl", "res/material_phong_frag.glsl");
        Shader lightShader = Shader::LoadFromFile("res/vert.glsl", "res/light_frag.glsl");
        // Loading the textures
        Texture diffuse = Texture::LoadFromFile("res/container.png");
        Texture specular = Texture::LoadFromFile("res/container_specular.png");
        // The projection matrix
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        // The view matrix will be generated by the camera
        Camera camera{ { -0.75f, 0.79f, 1.2f }, 3.2f, 120.f };
        userPtr.cameraPtr = &camera;
        // The model matrix
        glm::mat4 model{ 1.f };
        // The light's properties
        glm::vec3 lightPos{ 1.2f, 1.5f, -2.f };
        glm::vec3 lightColor{ 1.f };
        glm::mat4 lightModel{ 1.f };
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(.2f));
        // Using the time to calculate the time delta between frames
        double past = glfwGetTime();
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            double now = glfwGetTime();
            userPtr.deltaTime = past - now;
            past = now;
            // Processing keyboard inputs
            processInputs(window);
            // Clearing the color channel of the current framebuffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Setting the shader to active
            containerShader.UseProgram();
            // Setting shader variables

            // containerShader.SetFloat3("lightPos", lightPos);

            containerShader.SetFloat3("light.position", lightPos);

            containerShader.SetFloat3("lightColor", lightColor);
            containerShader.SetFloat3("camPos", camera.GetPosition());

            // containerShader.SetFloat3("color", { 1.f, .5f, .3f });

            diffuse.Bind(0);
            containerShader.SetInt("material.diffuse", 0);
            specular.Bind(1);
            containerShader.SetInt("material.specular", 1);
            containerShader.SetFloat("material.shininess", 128.f);

            // containerShader.SetFloat3("lightColor", lightColor);

            containerShader.SetFloat3("light.ambient", lightColor * .2f);
            containerShader.SetFloat3("light.diffuse", lightColor * .5f);
            containerShader.SetFloat3("light.specular", lightColor * 1.f);

            // Setting other uniforms in the shader
            containerShader.SetFloat("uTime", now);

            // Setting the projection and model in the shader
            containerShader.SetMatrix4("uProj", proj);
            containerShader.SetMatrix4("uView", camera.GetViewMatrix());
            containerShader.SetMatrix4("uModel", model);

            // Binding the rectangle object
            glBindVertexArray(vao);
            // Drawing the rectangle using the currently active shader
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

            // Drawing the light source
            lightShader.UseProgram();
            lightShader.SetMatrix4("uProj", proj);
            lightShader.SetMatrix4("uView", camera.GetViewMatrix());
            lightShader.SetMatrix4("uModel", lightModel);
            lightShader.SetFloat3("color", lightColor);
            glBindVertexArray(vbo);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

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
