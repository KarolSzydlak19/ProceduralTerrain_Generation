#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GuiWindow {
public:
	GuiWindow();
	void init(GLFWwindow* window);
	void terrain();
	void camera();
	void textures();
	void light();
	void terrainExport();
	void test();
};