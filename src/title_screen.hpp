#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "game_state.hpp"

class TitleScreen: public GameState {
public:
    TitleScreen(sf::RenderWindow& window);
    ~TitleScreen() = default;

    std::shared_ptr<GameState> Run() override;
};