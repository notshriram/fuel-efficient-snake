#include "snake.hpp"

Snake::Snake(const sf::Vector2i& head, const sf::Vector2i& tail) {
    body.push_back(tail);
    body.push_back(head);
}

void Snake::Update() {
    // update the position
    auto head = body.back();
    head += velocity;

    // update the body
    body.push_back(head);
    body.erase(body.begin());

}