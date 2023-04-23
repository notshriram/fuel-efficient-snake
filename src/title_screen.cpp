#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "title_screen.hpp"
#include "intro_screen.hpp"
#include <iostream>

TitleScreen::TitleScreen(sf::RenderWindow& window): GameState(window) {}

std::shared_ptr<GameState> TitleScreen::Run() {

    // load audio file from assets
    sf::SoundBuffer buffer;
    buffer.loadFromFile("assets/audio/title_bgm.mp3");
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
    sound.setLoop(true);

    // load background image from assets
    sf::Texture texture;
    if (!texture.loadFromFile("assets/images/title_screen.png"))
    {
        std::cerr << "Error loading title screen image" << std::endl;
    }

    sf::Sprite sprite(texture);
    sprite.setScale(window.getSize().x / sprite.getLocalBounds().width, window.getSize().y / sprite.getLocalBounds().height);



    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Sharp Retro/Sharp Retro.ttf"))
    {
        std::cerr << "Error loading font" << std::endl;
    }
    auto title = sf::Text{ "Fuel Efficient Snake", font, 50u };
    // center the title
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);

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
                        sound.stop();
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
            textOptions[i].setPosition(window.getSize().x / 2 - textOptions[i].getGlobalBounds().width / 2, 200 + i * 50);
        }

        window.clear();
        window.draw(sprite);
        window.draw(title);
        for (auto i = 0u; i < textOptions.size(); i++) {
            window.draw(textOptions[i]);
        }
        window.display();
    }
    sound.stop();
    return nullptr;
}