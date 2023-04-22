#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>
#include "node.hpp"

struct Snake
{
    Snake(size_t grid_width, size_t grid_height, std::shared_ptr<sf::Vector2i> food);
    ~Snake() = default;

    void Update(std::function <void()> callback);
    void Draw(sf::RenderWindow& window);

    size_t grid_width, grid_height;

    std::shared_ptr<sf::Vector2i> food;
    std::vector<sf::Vector2i> body;
    sf::Vector2i velocity = sf::Vector2i{ 0, 0 };
};
