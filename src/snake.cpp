#include "snake.hpp"
#include <iostream>
#include <cstdlib> // for rand

Snake::Snake(size_t grid_width, size_t grid_height, std::shared_ptr<sf::Vector2i> food): food(food), grid_width(grid_width), grid_height(grid_height) {

    sf::Vector2i head = sf::Vector2i{
        static_cast<int>(std::rand() % grid_width + 1),
        static_cast<int>(std::rand() % grid_height + 1) };
    sf::Vector2i tail = sf::Vector2i{ head.x - 1, head.y };

    body.push_back(tail);
    body.push_back(head);
}

void Snake::Update(std::function <void()> callback) {
    // update the position
    auto head = body.back();

    if (head.x == 0 && velocity.x == -1) {
        head.x = grid_width - 1;
    }
    else if (head.x == grid_width - 1 && velocity.x == 1) {
        head.x = 0;
    }
    else if (head.y == 0 && velocity.y == -1) {
        head.y = grid_height - 1;
    }
    else if (head.y == grid_height - 1 && velocity.y == 1) {
        head.y = 0;
    }
    else {
        head += velocity;
    }

    // update the body
    body.push_back(head);
    body.erase(body.begin());

    // check if the snake ate the food
    if (head == *food) {
        body.insert(body.begin(), head);
        callback();
    }
}