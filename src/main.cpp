#include <SFML/Graphics.hpp>
#include "game_state.hpp"
#include "title_screen.hpp"

int main()
{
    auto window = sf::RenderWindow{ { 1280u, 720u }, "Fuel Efficient Snake" };
    window.setFramerateLimit(144);

    std::shared_ptr<GameState> state = std::static_pointer_cast<GameState>(std::make_shared<TitleScreen>(window));
    while (window.isOpen() && state != nullptr)
    {
        state = state->Run();
    }
}