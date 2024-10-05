#include <iostream>
#include <glad/glad.h>     // For OpenGL function loading
#include <GLFW/glfw3.h>    // For creating windows and handling input

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

GLfloat vertices[] = {
	-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
	 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
	-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
	 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
	 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
};
GLuint indices[] = {
	0, 3, 5,
	3, 2, 4,
	5, 4, 1
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

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
	
	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {

		// Clear the screen with the same background color for each frame
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use the shader program
		shaderProgram.Activate();
		glUniform1f(uniID, 0.5f);

		VAO1.Bind();
		// Draw the triangle using the vertex data in the VAO
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		// Swap buffers to display the rendered frame
		glfwSwapBuffers(window);

		// Poll for and process events (keyboard, mouse, etc.)
		glfwPollEvents();
	}

	// Cleanup: Delete VAO, VBO, and the shader program
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	// Destroy the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
