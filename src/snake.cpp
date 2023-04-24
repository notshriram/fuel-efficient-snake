#include "snake.hpp"
#include <iostream>
#include <cstdlib> // for rand

Snake::Snake(size_t grid_width, size_t grid_height, std::shared_ptr<sf::Vector2i> food): food(food), grid_width(grid_width), grid_height(grid_height) {
    if (grid_width==0 || grid_height==0){
        return;
    }
    sf::Vector2i head = sf::Vector2i{
        static_cast<int>(std::rand() % grid_width + 1),
        static_cast<int>(std::rand() % grid_height + 1) };
    sf::Vector2i tail = sf::Vector2i{ head.x - 1, head.y };

    body.push_back(tail);
    body.push_back(head);
}

void Snake::Update(std::function <void()> callback, bool& is_game_over) {

    if (velocity.x == 0 && velocity.y == 0) {
        return;
    }

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
        // check if going backwards
        if (body.size() > 1) {
            auto neck = body.end() - 2;
            if (head.x + velocity.x == neck->x && head.y + velocity.y == neck->y) {
                velocity = sf::Vector2i{ 0, 0 };
                return;
            }
        }
        // check if the snake hit itself
        for (auto it = body.begin(); it != body.end() - 1; ++it) {
            if (head.x + velocity.x == it->x && head.y + velocity.y == it->y) {
                is_game_over = true;
                return;
            }
        }
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