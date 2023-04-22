#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "node.hpp"
#include "game_state.hpp"
#include "snake.hpp"

class GameScreen: public GameState {
private:
    void Init();
    void Draw();
    std::vector<std::vector<std::shared_ptr<Node>>> grid;
    void SolveAStar(std::shared_ptr<Node> start, std::shared_ptr<Node> end);
    Snake snake;

public:
    GameScreen(sf::RenderWindow& window);
    ~GameScreen() = default;

    std::shared_ptr<GameState> Run() override;
};