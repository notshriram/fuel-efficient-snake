#include "game_over.hpp"
#include "title_screen.hpp"

#include <iostream>

GameOver::GameOver(sf::RenderWindow& window, int score, int in_game_time): GameState(window), score(score), in_game_time(in_game_time) {}

std::shared_ptr<GameState> GameOver::Run()
{
    int window_width = window.getSize().x;
    int window_height = window.getSize().y;

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Sharp Retro/Sharp Retro.ttf"))
    {
        std::cerr << "Error loading font Sharp Retro\n";
        exit(1);
    }
    sf::Font font2;
    if (!font2.loadFromFile("assets/fonts/DotGothic16/DotGothic16-Regular.ttf"))
    {
        std::cerr << "Error loading font DotGothic16\n";
        exit(1);
    }

    std::string status;
    std::string message;
    int salary = 0;

    if (in_game_time >= 24 * 60)
    {
        status = "Shift Ended!";
        message = "You have completed your shift! Here is your salary: ";
        salary = 500 - score;
    }
    else
    {
        status = "Crash";
        message = "You done totaled your lorry and lost your job! Your salary is: ";
        salary = score;
    }

    sf::Text status_text;
    status_text.setFont(font);
    status_text.setCharacterSize(64);
    status_text.setFillColor(sf::Color::White);
    status_text.setString(status);
    status_text.setPosition(window_width / 2 - status_text.getGlobalBounds().width / 2, 100);

    sf::Text salary_text;
    salary_text.setFont(font2);
    salary_text.setCharacterSize(32);
    salary_text.setFillColor(sf::Color::White);
    salary_text.setString(message);
    salary_text.setPosition(window_width / 2 - salary_text.getGlobalBounds().width / 2, 200);

    sf::Text salary_amount_text;
    salary_amount_text.setFont(font2);
    salary_amount_text.setCharacterSize(32);
    if (salary < 0)
    {
        salary_amount_text.setFillColor(sf::Color::Red);
    }
    else
    {
        salary_amount_text.setFillColor(sf::Color::Green);
    }
    salary_amount_text.setString(std::to_string(salary));
    salary_amount_text.setPosition(window_width / 2 - salary_amount_text.getGlobalBounds().width / 2, 250);


    sf::Text play_again_text;
    play_again_text.setFont(font2);
    play_again_text.setCharacterSize(32);
    play_again_text.setFillColor(sf::Color::White);
    play_again_text.setString("Press Space to play again");
    play_again_text.setPosition(window_width / 2 - play_again_text.getGlobalBounds().width / 2, 300);

    sf::Text quit_text;
    quit_text.setFont(font2);
    quit_text.setCharacterSize(32);
    quit_text.setFillColor(sf::Color::White);
    quit_text.setString("Press Escape to quit");
    quit_text.setPosition(window_width / 2 - quit_text.getGlobalBounds().width / 2, 350);

    while (window.isOpen())
    {
        auto event = sf::Event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Return)
                {
                    return std::make_shared<TitleScreen>(window);
                }
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        window.clear();

        window.draw(status_text);
        window.draw(salary_text);
        window.draw(salary_amount_text);
        window.draw(play_again_text);
        window.draw(quit_text);

        window.display();
    }

    return nullptr;
}

