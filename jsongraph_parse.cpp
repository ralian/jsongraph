#include "jsongraph_parse.hpp"

#include <cstdio>
#include <cstring>
#include <rapidjson/document.h>
#include <string>

namespace jsongraph {

auto parse_node = [](const char* name, const auto& val) -> graph_node {
    std::vector<graph_node> value;

    for (const auto& member : val) {
        graph_node inner;
        inner.key = member.name.GetString();
        switch (member.value.GetType())
        {
            case rapidjson::Type::kNullType: break;
            case rapidjson::Type::kFalseType: inner.value = false; break;
            case rapidjson::Type::kTrueType: inner.value = true; break;
            case rapidjson::Type::kObjectType: break;
            case rapidjson::Type::kArrayType: break;
            case rapidjson::Type::kStringType: inner.value = member.value.GetString(); break;
            case rapidjson::Type::kNumberType: inner.value = member.value.GetDouble(); break; // TODO int handling
            default: break; // throw here?
        }
        value.emplace_back(inner);
    }

    return {name, {}, IM_COL32_WHITE, value};
};

auto parse_edge = [](const auto& val) -> graph_edge {
    graph_edge e;
    e.output_name = val[0].GetString();
    e.output_key = val[1].GetString();
    e.input_name = val[2].GetString();
    e.input_key = val[3].GetString();
    return e;
};

bool parse_file(const char* path, std::vector<graph_node>& nodes, std::vector<graph_edge>& edges) {
    // todo buffer this
    FILE* fp = std::fopen(path, "rb");
    if (!fp) return false;

    int c; std::string contents;
    while ((c = std::fgetc(fp)) != EOF)
        contents += c;

    if (std::ferror(fp))
        return false; // is not closing here catastrophic?

    std::fclose(fp);

    rapidjson::Document doc;
    doc.Parse(contents.c_str());

    for (const auto& m : doc.GetObject()) {
        const char* name = m.name.GetString();
        if (name[0] != '$') // This throws out all special nodes like $edges or $comment
            nodes.emplace_back(parse_node(name, m.value.GetObject()));
        else if (std::strcmp(name, "$edges") == 0)
            for (const auto& edge_val : m.value.GetArray())
                edges.emplace_back(parse_edge(edge_val.GetArray()));
    }
    
    return true;
}

}
