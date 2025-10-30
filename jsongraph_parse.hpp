#include "jsongraph.hpp"

#include <cstdio>
#include <rapidjson/document.h>
#include <string>

namespace jsongraph {

graph_node parse_node(const rapidjson::GenericObject<false, rapidjson::Value>& val) { // todo true for const
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

    return {"", {}, IM_COL32_WHITE, value};
}

graph_edge parse_edge(const rapidjson::GenericValue<rapidjson::UTF8<>>& val) {
    graph_edge e;
    return e;
}

bool parse_file(const char* path, std::vector<graph_node>& nodes, std::vector<graph_edge> edges) {
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

    for (const auto& m : doc.GetObject())
        nodes.emplace_back(parse_node(m.value.GetObject()));
    
    return true;
}

}
