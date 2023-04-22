#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "game_state.hpp"

class IntroScreen: public GameState {
public:
    IntroScreen(sf::RenderWindow& window);
    ~IntroScreen() = default;

    std::shared_ptr<GameState> Run() override;
};