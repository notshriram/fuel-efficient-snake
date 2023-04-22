#pragma once
#include <memory>
#include <SFML/Graphics.hpp>

// Game State class which will be inherited by all game states
// such as the main menu, the game, the pause menu, etc.

class GameState {
protected:
    sf::RenderWindow& window;

public:
    GameState(sf::RenderWindow& window): window(window) {};
    virtual ~GameState() = default;

    virtual std::shared_ptr<GameState> Run() = 0;
};