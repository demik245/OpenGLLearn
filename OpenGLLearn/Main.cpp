#include <iostream>
#include <glad/glad.h>     // For OpenGL function loading
#include <GLFW/glfw3.h>   // For creating windows and handling input
#include <stb/stb_image.h> // For loading textures
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "shaderClass.h"   // Custom shader class
#include "VAO.h"           // Custom Vertex Array Object class
#include "VBO.h"           // Custom Vertex Buffer Object class
#include "EBO.h"           // Custom Element Buffer Object class

// Vertex data (coordinates, colors, texture coordinates)
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};

// Indices for vertices order
GLuint indices[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};

const unsigned int width = 800;
const unsigned int height = 800;

int main() {
    // Initialize GLFW library
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW for OpenGL version 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window with GLFW
    GLFWwindow* window = glfwCreateWindow(width, height, "Moje pierwsze OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();  // Terminate GLFW if window creation failed
        return -1;
    }
    glfwMakeContextCurrent(window);  // Set the current context to the created window

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Define the viewport dimensions (size of the rendering window)
    glViewport(0, 0, width, height);

    // Create and compile shader program
    Shader shaderProgram("default.vert", "default.frag");

    // Create and bind VAO
    VAO VAO1;
    VAO1.Bind();

    // Create VBO and EBO
    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    // Link vertex attributes to the VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);        // Position
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Color
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coords
    VAO1.Unbind();  // Unbind the VAO
    VBO1.Unbind();  // Unbind the VBO
    EBO1.Unbind();  // Unbind the EBO

    // Get uniform locations for shader
    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    Texture pw("pw.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    pw.texUnit(shaderProgram, "tex0", 0);

    float rotation = 0.0f;
    double prevTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen with a background color
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate the shader program and set the uniform scale
        shaderProgram.Activate();

        double crntTime = glfwGetTime();
        if (crntTime - prevTime >= 1 / 60) {
            rotation += 0.5f;
            prevTime = crntTime;
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.5f));
        proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        glUniform1f(uniID, 0.5f); // Set scale uniform

        // Bind the texture and VAO, then draw the elements
        pw.Bind();
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0); // Draw the triangles

        // Swap buffers to display the rendered frame
        glfwSwapBuffers(window);

        // Poll for and process events (keyboard, mouse, etc.)
        glfwPollEvents();
    }

    // Cleanup: Delete VAO, VBO, and EBO, and shader program
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    pw.Delete();
    shaderProgram.Delete();

    // Destroy the window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
