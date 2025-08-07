#include <string>
#include <variant>
#include <vector>

#include "jsongraph.hpp"

namespace jsongraph {

struct graph_edge {
    // Don't get confused here, each edge is directed from (node) output -> (node) input
    std::string output_name, output_label;
    std::string input_name, input_label;
    ImVec2 out, in; // These are updated on frame and edges should be rendered after nodes.
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

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

void visit_trampoline(graph_node& node);

const auto visitor = overloads {
    [](const std::string& val){ ImGui::Text(val.c_str()); },
    [](bool & val){ ImGui::Checkbox("##", &val); },
    [](int & val){ ImGui::InputInt("##", &val); },
    [](double & val){ ImGui::InputDouble("##", &val); },
    [](std::vector<graph_node>& val){
        for (graph_node& node : val)
            visit_trampoline(node);
    }
};

// There must be a better way to do this.
void visit_trampoline(graph_node& node) {
    std::visit(visitor, node.value);
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
        std::vector<graph_node> {
            {
                "C",
                {},
                {1.f, 1.f, 1.f, 1.f},
                3.14
            },
            {
                "C",
                {},
                {1.f, 1.f, 1.f, 1.f},
                -1.0/12.0
            }
        }
    }
};
std::vector<graph_edge> edge_list;

bool is_dragging = false;
std::string new_edge_origin_window;
ImVec2 edge_origin_window_pos;
graph_edge new_edge;

// Interior check for dragging and dropping. We don't want to do a sphere check
// here - we want it to be fast and err on the side of allowing the drop to happen.
inline auto points_overlap = [](ImVec2 a, ImVec2 b, float buffer) -> bool {
    if (a.x - b.x > buffer || b.x - a.x > buffer) return false;
    if (a.y - b.y > buffer || b.y - a.y > buffer) return false;
    return true;
};

void render_edge(const graph_edge& e) {
    float dist = sqrt((e.in.x - e.out.x)*(e.in.x - e.out.x)
                    + (e.in.y - e.out.y)*(e.in.y - e.out.y));
    auto outvec = ImVec2(e.out.x + (dist*0.5), e.out.y);
    auto invec = ImVec2(e.in.x - (dist*0.5), e.in.y);
    ImGui::GetForegroundDrawList()->AddBezierCubic(e.out, outvec, invec, e.in, IM_COL32_WHITE, 2.5f);
}

void render_node(GLFWwindow* window, editor_state& state, graph_node& node)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io; // TODO Does this involve a virtual function call? If not, leave as is.

    if (node.key == new_edge_origin_window)
        ImGui::SetNextWindowPos(edge_origin_window_pos);

    const auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::Begin(node.key.c_str(), nullptr, flags);
    std::visit(visitor, node.value);

    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    const ImVec2 cur_pos = ImGui::GetCursorScreenPos();
    const ImVec2 win_pos = ImGui::GetWindowPos();
    const ImVec2 win_size = ImGui::GetWindowSize();
    
    // why does 3/5 look better than 1/2?
    float ylevel = cur_pos.y - 3*ImGui::GetTextLineHeight()/5;
    if (ImGui::IsWindowCollapsed())
        ylevel = win_pos.y + (win_size.y / 2);

    auto in = ImVec2(win_pos.x, ylevel);
    auto out = ImVec2(win_pos.x + win_size.x, ylevel);

    float r = 5.0f;
    draw_list->AddCircle(out, r, node.color, 16, 2.0f);
    draw_list->AddCircle(in, r, node.color, 16, 2.0f);

    for (graph_edge& edge : edge_list) {
        if (edge.output_name == node.key)
            edge.out = out;
        if (edge.input_name == node.key)
            edge.in = in;
    }

    if (io.MouseDown[0] && points_overlap(io.MouseClickedPos[0], out, r)) {
        is_dragging = true;

        // Lock the window
        new_edge_origin_window = node.key;
        edge_origin_window_pos = ImGui::GetWindowPos();
        
        new_edge.output_name = node.key;
        new_edge.out = out;
        new_edge.in = io.MousePos;
    } else if (is_dragging && !io.MouseDown[0]
            && node.key != new_edge.output_name // No. Just no.
            && points_overlap(io.MousePos, in, r)) {
        is_dragging = false;
        new_edge_origin_window = ""; // clear window lock
        new_edge.input_name = node.key;
        new_edge.in = in;
        edge_list.push_back(new_edge);
        new_edge = graph_edge{};
    }

    ImGui::End();
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

    for (const graph_edge& edge : edge_list)
        render_edge(edge);
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    is_dragging = is_dragging && io.MouseDown[0];
    if (is_dragging)
        render_edge(new_edge);

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
