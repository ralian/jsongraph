#include <string>
#include <variant>
#include <vector>

#include "jsongraph.hpp"

namespace jsongraph {

struct graph_edge {
    std::string output_name;
    std::string name;
    std::string label;
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
        std::vector<graph_node*>, // We are an object (or array) - but that is treated as an object with numeric key strings!
        graph_edge
    > value;
};

std::vector<graph_node> node_list {
    {
        "A",
        {},
        {1.f, 1.f, 1.f, 1.f},
        "Test String"
    },
    {
        "B",
        {},
        {1.f, 1.f, 1.f, 1.f},
        true
    },
    {
        "C",
        {},
        {1.f, 1.f, 1.f, 1.f},
        42
    },
    {
        "D",
        {},
        {1.f, 1.f, 1.f, 1.f},
        3.14
    },
    {
        "E",
        {},
        {1.f, 1.f, 1.f, 1.f},
        graph_edge{"My Connection", "", ""}
    }
};
std::vector<graph_edge> edge_list;

std::string new_edge_origin;
graph_edge new_edge;

int render_node(GLFWwindow* window, editor_state& state, graph_node& node)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io; // TODO Does this involve a virtual function call? If not, leave as is.

    const auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::Begin(node.key.c_str(), nullptr, flags);
    // TODO visitor
    switch (node.value.index()) {
        case 0:
            ImGui::Text(std::get<std::string>(node.value).c_str());
            break;
        case 1:
            ImGui::Checkbox("##", &std::get<bool>(node.value));
            break;
        case 2:
            ImGui::InputInt("##", &std::get<int>(node.value));
            break;
        case 3:
            ImGui::InputDouble("##", &std::get<double>(node.value));
            break;
        case 4:
            break;
        case 5:
            auto& edge = std::get<graph_edge>(node.value);
            ImGui::Text(edge.output_name.c_str());
            break;
    }
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    const ImVec2 cur_pos = ImGui::GetCursorScreenPos();
    const ImVec2 win_pos = ImGui::GetWindowPos();
    const ImVec2 win_size = ImGui::GetWindowSize();
    
    // why does 3/5 look better than 1/2?
    auto start = ImVec2(win_pos.x + win_size.x, cur_pos.y - 3*ImGui::GetTextLineHeight()/5);

    float r = 5.0f;
    if (!ImGui::IsWindowCollapsed()) {
        draw_list->AddCircle(start, r, node.color, 16, 2.0f);
        if (io.MouseDown[0]
        && io.MouseClickedPos[0].x < (start.x + r) // todo this is stupid
        && io.MouseClickedPos[0].x > (start.x - r)
        && io.MouseClickedPos[0].y < (start.y + r)
        && io.MouseClickedPos[0].y > (start.y - r)
        ) {
            float dist = sqrt((io.MousePos.x - start.x)*(io.MousePos.x - start.x)
                        + (io.MousePos.y - start.y)*(io.MousePos.y - start.y));
            auto outvec = ImVec2(start.x + (dist*0.5), start.y);
            auto invec = ImVec2(io.MousePos.x - (dist*0.5), io.MousePos.y);
            ImGui::GetForegroundDrawList()->AddBezierCubic(start, outvec, invec, io.MousePos, node.color, 2.5f);
        }
    }
    ImGui::End();

    return 0;
}

int render(GLFWwindow* window, editor_state& state)
{
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return -1;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (state.show_demo_window)
        ImGui::ShowDemoWindow(&state.show_demo_window);

    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("Save", "CTRL+S");
        ImGui::MenuItem("Open", "CTRL+O");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Debug"))
    {
        ImGui::Checkbox("ImGui Demo Window", &state.show_demo_window);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    for (graph_node& node : node_list)
        render_node(window, state, node);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(state.clear_color.x * state.clear_color.w, state.clear_color.y * state.clear_color.w, state.clear_color.z * state.clear_color.w, state.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    return 0;
}

}
