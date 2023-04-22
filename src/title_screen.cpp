#include <SFML/Graphics.hpp>
#include "title_screen.hpp"
#include "intro_screen.hpp"
#include <iostream>

TitleScreen::TitleScreen(sf::RenderWindow& window): GameState(window) {}

std::shared_ptr<GameState> TitleScreen::Run() {

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Sharp Retro/Sharp Retro.ttf"))
    {
        std::cerr << "Error loading font" << std::endl;
    }
    auto title = sf::Text{ "Fuel Efficient Snake", font, 50u };

    std::vector<sf::String> options = { "Play", "Settings", "Quit" };

    int selectedOption = 0;

    // inside the main loop, between window.clear() and window.display()
    while (window.isOpen()) {
        auto event = sf::Event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Return) {
                    if (selectedOption == 0) {
                        return std::make_shared<IntroScreen>(window);
                    }
                    if (selectedOption == 2) {
                        window.close();
                    }
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::Down) {
                    selectedOption = (selectedOption + 1) % options.size();
                }
                if (event.key.code == sf::Keyboard::Up) {
                    selectedOption = (selectedOption - 1 + options.size()) % options.size();
                }
            }
        }
        std::vector<sf::Text> textOptions;
        for (auto i = 0u; i < options.size(); i++) {
            if (i == selectedOption) {
                textOptions.push_back(sf::Text{ options[i], font, 35u});
                textOptions[i].setFillColor(sf::Color::Red);
            }
            else
                textOptions.push_back(sf::Text{ options[i], font, 30u });
            textOptions[i].setPosition(100, 100 + i * 50);
        }

        window.clear();
        window.draw(title);
        for (auto i = 0u; i < textOptions.size(); i++) {
            window.draw(textOptions[i]);
        }
        window.display();
    }

    return nullptr;
}