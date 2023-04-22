#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "node.hpp"


struct Snake
{
    Snake(const sf::Vector2i& head, const sf::Vector2i& tail);
    ~Snake() = default;

    void Update();
    void Draw(sf::RenderWindow& window);

    std::vector<sf::Vector2i> body;
    sf::Vector2i velocity;
};
