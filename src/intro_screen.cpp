#include "intro_screen.hpp"
#include "game_screen.hpp"
#include <memory>
#include <fstream>
#include <algorithm>

IntroScreen::IntroScreen(sf::RenderWindow& window): GameState(window) {

}

std::vector<sf::Text> readFromFile(std::string filename, int max_length = 100) {
    std::ifstream file(filename);
    std::string str;
    std::string line;
    std::vector<sf::Text> text;
    if (file.is_open()) {
        while (getline(file, line)) {
            //if line is longer than 80 characters, split it into multiple lines
            std::string temp = line;
            while (temp.length() > max_length) {
                // find the last space in the first 80 characters
                int last_space = temp.substr(0, max_length).find_last_of(" ");
                // split the line at the last space and remove the space
                line = temp.substr(0, last_space);
                temp = temp.substr(last_space + 1, temp.length() - 1);
                sf::Text txt;
                txt.setString(line);
                text.push_back(txt);
            }
            sf::Text txt;
            txt.setString(temp);
            text.push_back(txt);
        }
    }
    return text;
}

std::shared_ptr<GameState> IntroScreen::Run() {
    // load the font
    sf::Font font;
    unsigned int character_size = 20u;

    font.loadFromFile("assets/fonts/DotGothic16/DotGothic16-Regular.ttf");

    int max_length = character_size * 80 / 16;

    std::vector<sf::Text> joiningLetter = readFromFile("assets/text/joining_letter.txt", max_length);

    sf::Text title = sf::Text{ "A letter from HR", font, character_size * 3 / 2 };

    sf::Text call_to_action = sf::Text{ "Press space to continue", font, character_size };

    while (window.isOpen()) {
        auto event = sf::Event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    return std::make_shared<GameScreen>(window);
                }
            }
        }

        window.clear();
        // print title at the top of the screen centered
        title.setPosition(window.getSize().x / 2 - title.getLocalBounds().width / 2, character_size);
        window.draw(title);

        for (auto it = joiningLetter.begin(); it != joiningLetter.end(); it++) {
            int line_number = std::distance(joiningLetter.begin(), it);
            it->setFont(font);
            it->setCharacterSize(character_size);
            it->setFillColor(sf::Color::White);
            it->setPosition(character_size * 5, character_size * (5 + line_number));
            window.draw(*it);
        }
        // print call to action at the bottom of the screen centered
        call_to_action.setPosition(window.getSize().x / 2 - call_to_action.getLocalBounds().width / 2, window.getSize().y - call_to_action.getLocalBounds().height * 2);
        window.draw(call_to_action);
        window.display();
    }
    return nullptr;
}
