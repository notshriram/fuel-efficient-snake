#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "game_state.hpp"

class GameOver: public GameState {
private:
    int score;
    int extra_steps;
    int in_game_time;

public:
    GameOver(sf::RenderWindow& window, int score, int in_game_time);
    ~GameOver() = default;

    std::shared_ptr<GameState> Run() override;
};