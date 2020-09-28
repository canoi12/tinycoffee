#include "GL/gl3w.h"

#include "glfw/include/GLFW/glfw3.h"

int main(int argc, char ** argv) {
	if (!glfwInit()) return 0;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	GLFWwindow *window = glfwCreateWindow(640, 380, "teste", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (gl3wInit()) return 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}
}