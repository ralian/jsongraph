#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <stdio.h>
#include <string>
#include <variant>
#include <vector>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

namespace jsongraph {

struct graph_edge {
    // Don't get confused here, each edge is directed from (node) output -> (node) input
    std::string input_name, output_name;
    ImGuiID input_id, output_id;
    ImVec2 out, in; // These are updated on frame and edges should be rendered after nodes.
    ImColor color{IM_COL32_WHITE};
};

struct graph_node {
    std::string key{""};
    std::vector<std::string> tags{};
    ImColor color{1.f, 1.f, 1.f};

    // TODO split this into a smaller K/V json object that doesnt have tag/color data
    std::variant< // underlying json value type
        std::string,
        bool,
        int, // not sure what type to use for numeric json here
        double, // again, same problem
        std::vector<graph_node> // We are an object (or array) - but that is treated as an object with numeric key strings!
    > value;
};

struct editor_state {
    bool show_demo_window{false};
    ImVec4 clear_color{0.45f, 0.55f, 0.60f, 1.00f};
    std::string open_file; // string rather than path to avoid <filesystem> in header
};

int render(GLFWwindow* window, editor_state& state);

void drop_callback(GLFWwindow* window, int count, const char** paths);

}
