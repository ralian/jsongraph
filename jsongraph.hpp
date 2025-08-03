#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

namespace jsongraph {

struct editor_state {
    bool show_demo_window{false};
    ImVec4 clear_color{0.45f, 0.55f, 0.60f, 1.00f};
};

int render(GLFWwindow* window, editor_state& state);

}
