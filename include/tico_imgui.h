#ifndef TICO_IMGUI_H
#define TICO_IMGUI_H

#include "tico.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

TIC_API int tico_imgui_init(tc_Window *window);
TIC_API int tico_imgui_update();
TIC_API int tico_imgui_render();

TIC_API void tico_imgui_terminate();

#endif

#if defined(TICO_IMGUI_IMPLEMENTATION)

// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

int tico_imgui_init(tc_Window *window) {
	igCreateContext(NULL);

	const char *glsl_version = "#version 330";
	ImGui_ImplGlfw_InitForOpenGL(window->handle, 1);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGuiIO *io = igGetIO();
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io->ConfigDockingWithShift = 1;
	io->ConfigWindowsMoveFromTitleBarOnly = 1;

	LOG("ImGui Module initiated");

	return 1;
}

int tico_imgui_update() {
	ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  igNewFrame();

  return 1;
}

int tico_imgui_render() {
	igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
  return 1;
}

void tico_imgui_terminate() {
	ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGuiContext *ctx = igGetCurrentContext();
  igDestroyContext(ctx);
  LOG("ImGui Module terminated");
}

#endif