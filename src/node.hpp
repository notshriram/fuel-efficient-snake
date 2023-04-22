#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <limits>

struct Node {
    bool isWall = false;
    bool isVisited = false;
    bool isPath = false;
    float localGoal = std::numeric_limits<float>::infinity();
    float globalGoal = std::numeric_limits<float>::infinity();
    unsigned int x = 0;
    unsigned int y = 0;
    std::vector<std::shared_ptr<Node>> neighbours;
    std::shared_ptr<Node> parent;

    void Reset() {
        isVisited = false;
        isPath = false;
        localGoal = std::numeric_limits<float>::infinity();
        globalGoal = std::numeric_limits<float>::infinity();
    }
};
