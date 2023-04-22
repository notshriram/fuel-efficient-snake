#include "snake.hpp"

Snake::Snake(const sf::Vector2i& head, const sf::Vector2i& tail) {
    this->head = head;
    this->tail = tail;
    body.push_back(head);
    body.push_back(tail);
}

void Snake::Update() {
    // update the position
    head += velocity;

    // update the body
    body.push_back(head);
    body.erase(body.begin());
}