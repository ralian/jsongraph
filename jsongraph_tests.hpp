#pragma once

#include "jsongraph.hpp"

namespace jsongraph {

std::vector<graph_node> test_graph {{
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
                "A",
                {},
                {1.f, 1.f, 1.f, 1.f},
                3.14
            },
            {
                "B",
                {},
                {1.f, 1.f, 1.f, 1.f},
                -1.0/12.0
            }
        }
    }
}};

};