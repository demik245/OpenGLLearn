#include <iostream>
#include <glad/glad.h>     // For OpenGL function loading
#include <GLFW/glfw3.h>    // For creating windows and handling input
#include<stb/stb_image.h>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f,     1.0f, 0.0f,  0.0f,   0.0f, 0.0f,      // Lower left corner
	-0.5f, 0.5f, 0.0f,     0.0f, 1.0f,  0.0f,    0.0f, 1.0f,      // Lower right corner
	0.5f,  0.5f, 0.0f,     0.0f, 0.0f,  1.0f,    1.0f, 1.0f,      // Upper corner
	0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,     1.0f, 0.0f,      // Inner left
};
GLuint indices[] = {
	0, 2, 1,
	0, 3, 2
};

int main() {

	glfwInit();  // Initialize GLFW library

	// Configure GLFW for OpenGL version 3.3 core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Define vertices for a triangle

	// Create a window with GLFW
	GLFWwindow* window = glfwCreateWindow(800, 800, "Moje pierwsze OpenGL", NULL, NULL);

	// Check if window creation failed
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();  // Terminate GLFW if window creation failed
		return -1;
	}
	glfwMakeContextCurrent(window);  // Set the current context to the created window

	gladLoadGL();  // Load OpenGL function pointers using GLAD

	// Define the viewport dimensions (size of the rendering window)
	glViewport(0, 0, 800, 800);

	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
	



	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load("pw.png", &widthImg, &heightImg, &numColCh, 0);

	if (!bytes) {
		std::cerr << "Failed to load texture" << std::endl;
		return -1;
	}

	GLenum format;
	if (numColCh == 1)
		format = GL_RED;
	else if (numColCh == 3)
		format = GL_RGB;
	else if (numColCh == 4)
		format = GL_RGBA;
	else
		format = GL_RGB;  // Default fallback

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, format, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
	shaderProgram.Activate();
	glUniform1i(tex0Uni, 0);


	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {

		// Clear the screen with the same background color for each frame
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use the shader program
		shaderProgram.Activate();
		glUniform1f(uniID, 0.5f);
		glBindTexture(GL_TEXTURE_2D, texture);
		VAO1.Bind();
		// Draw the triangle using the vertex data in the VAO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap buffers to display the rendered frame
		glfwSwapBuffers(window);

		// Poll for and process events (keyboard, mouse, etc.)
		glfwPollEvents();
	}

	// Cleanup: Delete VAO, VBO, and the shader program
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
