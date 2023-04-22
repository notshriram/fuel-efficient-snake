#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "node.hpp"


struct Snake
{
    Snake(const sf::Vector2i& head, const sf::Vector2i& tail);
    ~Snake() = default;

    void Update();
    void Draw(sf::RenderWindow& window);

    std::vector<sf::Vector2i> body;
    sf::Vector2i position;
    sf::Vector2i velocity;

    sf::Vector2i head;
    sf::Vector2i tail;
};
