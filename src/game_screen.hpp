#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "node.hpp"
#include "game_state.hpp"
#include "snake.hpp"

class GameScreen: public GameState {
private:
    void Draw();
    const int grid_size = 30;
    std::vector<std::vector<std::shared_ptr<Node>>> grid;
    void SolveAStar(std::shared_ptr<Node> start, std::shared_ptr<Node> end);
    Snake snake;
    std::shared_ptr<sf::Vector2i> food;

public:
    GameScreen(sf::RenderWindow& window);
    ~GameScreen() = default;

    std::shared_ptr<GameState> Run() override;
};