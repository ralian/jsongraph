#pragma once

#include "jsongraph.hpp"

namespace jsongraph {
bool parse_file(const char* path, std::vector<graph_node>& nodes, std::vector<graph_edge>& edges);
}
