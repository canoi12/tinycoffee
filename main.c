// #define TICO_NO_RENDER_MODULE
#include "tico.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

int main(int argc, char ** argv) {
	tc_Config config = tico_config_load("project.tico", argc, argv);

	tico_init(&config);
	tico_main_loop();
	tico_terminate();

	// if (!glfwInit()) {
	// 	printf("Failed to init GLFW");
	// 	return 0;
	// }

	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// GLFWwindow *window = glfwCreateWindow(640, 380, "titulow", NULL, NULL);
	// glfwMakeContextCurrent(window);

	// if (gl3wInit()) {
	// 	printf("Failed to init gl3w");
	// 	return 0;
	// }

	// while (!glfwWindowShouldClose(window)) {
	// 	glfwPollEvents();

	// 	glClearColor(0, 0, 0, 0);
	// 	glClear(GL_COLOR_BUFFER_BIT);

	// 	glfwSwapBuffers(window);
	// }

	// glfwDestroyWindow(window);
	// glfwTerminate();

	return 0;
}