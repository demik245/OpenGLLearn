#include <iostream>
#include <glad/glad.h>     // For OpenGL function loading
#include <GLFW/glfw3.h>   // For creating windows and handling input
#include <stb/stb_image.h> // For loading textures

#include "shaderClass.h"   // Custom shader class
#include "VAO.h"           // Custom Vertex Array Object class
#include "VBO.h"           // Custom Vertex Buffer Object class
#include "EBO.h"           // Custom Element Buffer Object class

// Vertex data (coordinates, colors, texture coordinates)
GLfloat vertices[] = {
    // Position        // Color           // Texture Coords
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  0.0f,  0.0f, 0.0f,  // Lower left corner
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  // Upper left corner
     0.5f,  0.5f, 0.0f,  0.0f, 0.0f,  1.0f,  1.0f, 1.0f,  // Upper right corner
     0.5f, -0.5f, 0.0f,  1.0f, 1.0f,  1.0f,  1.0f, 0.0f   // Lower right corner
};

// Element indices for drawing triangles
GLuint indices[] = {
    0, 1, 2, // First triangle
    0, 2, 3  // Second triangle
};

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
    GLFWwindow* window = glfwCreateWindow(800, 800, "Moje pierwsze OpenGL", NULL, NULL);
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
    glViewport(0, 0, 800, 800);

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

    // Load texture
    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true); // Flip the texture vertically on load
    unsigned char* bytes = stbi_load("pw.png", &widthImg, &heightImg, &numColCh, 0);

    if (!bytes) {
        std::cerr << "Failed to load texture" << std::endl;
        return -1;
    }

    // Determine texture format based on number of color channels
    GLenum format;
    if (numColCh == 1) {
        format = GL_RED;
    }
    else if (numColCh == 3) {
        format = GL_RGB;
    }
    else if (numColCh == 4) {
        format = GL_RGBA;
    }
    else {
        format = GL_RGB;  // Default fallback
    }

    // Generate and configure texture
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Upload texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for the texture

    // Free texture data
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    // Set the texture uniform in the shader
    GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
    shaderProgram.Activate();
    glUniform1i(tex0Uni, 0); // Set the texture unit to 0

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen with a background color
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate the shader program and set the uniform scale
        shaderProgram.Activate();
        glUniform1f(uniID, 0.5f); // Set scale uniform

        // Bind the texture and VAO, then draw the elements
        glBindTexture(GL_TEXTURE_2D, texture);
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw the triangles

        // Swap buffers to display the rendered frame
        glfwSwapBuffers(window);

        // Poll for and process events (keyboard, mouse, etc.)
        glfwPollEvents();
    }

    // Cleanup: Delete VAO, VBO, and EBO, and shader program
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    glDeleteTextures(1, &texture);
    shaderProgram.Delete();

    // Destroy the window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
