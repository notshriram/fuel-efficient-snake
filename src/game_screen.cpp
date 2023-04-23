#include <iostream>
#include <vector>
#include <list>
#include <numeric>
#include <cstdlib> // for rand
#include <cmath>
#include <algorithm>
#include <memory>
#include <SFML/Audio.hpp>

#include "node.hpp"
#include "game_screen.hpp"
#include "title_screen.hpp"
#include "snake.hpp"
#include "game_over.hpp"

inline std::string get_formatted_time(unsigned int ticks)
{
    unsigned int hours = ticks / 60;
    unsigned int seconds = ticks % 60;
    std::string hours_str = hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);
    std::string seconds_str = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
    return hours_str + ":" + seconds_str;
}

GameScreen::GameScreen(sf::RenderWindow& window):
    GameState(window),
    grid(0),
    snake(Snake(0, 0, nullptr)) {
    // initialize the game grid
    const unsigned int grid_width = window.getSize().x / grid_size;
    const unsigned int grid_height = window.getSize().y / grid_size;

    food = std::make_shared<sf::Vector2i>(std::rand() % grid_width, std::rand() % grid_height);

    this->grid = std::vector<std::vector<std::shared_ptr<Node>>>(grid_width, std::vector<std::shared_ptr<Node>>(grid_height));

    for (unsigned int i = 0; i < grid_width; ++i) {
        for (unsigned int j = 0; j < grid_height; ++j) {
            grid[i][j] = std::make_shared<Node>();
            grid[i][j]->x = i;
            grid[i][j]->y = j;
        }
    }

    // initialize the neighbours
    for (unsigned int i = 0; i < grid_width; ++i) {
        for (unsigned int j = 0; j < grid_height; ++j) {
            auto node = grid[i][j];
            if (i > 0) {
                node->neighbours.push_back(grid[i - 1][j]);
            }
            if (i < grid_width - 1) {
                node->neighbours.push_back(grid[i + 1][j]);
            }
            if (j > 0) {
                node->neighbours.push_back(grid[i][j - 1]);
            }
            if (j < grid_height - 1) {
                node->neighbours.push_back(grid[i][j + 1]);
            }

            // loop around the edges
            if (i == 0) {
                node->neighbours.push_back(grid[grid_width - 1][j]);
            }
            if (i == grid_width - 1) {
                node->neighbours.push_back(grid[0][j]);
            }
            if (j == 0) {
                node->neighbours.push_back(grid[i][grid_height - 1]);
            }
            if (j == grid_height - 1) {
                node->neighbours.push_back(grid[i][0]);
            }
        }
    }

    this->snake = Snake(grid_width, grid_height, food);

    std::shared_ptr<Node> start = grid[snake.body.front().x][snake.body.front().y];
    std::shared_ptr<Node> end = grid[food->x][food->y];

    SolveAStar(start, end);

    shortest_distance = 0;
    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = 0; j < grid[i].size(); ++j) {
            if (grid[i][j]->isPath) {
                shortest_distance++;
            }
        }
    }
}

void GameScreen::Draw() {
    const unsigned int grid_width = window.getSize().x / grid_size;
    const unsigned int grid_height = window.getSize().y / grid_size;

    std::shared_ptr<Node> start = grid[snake.body.front().x][snake.body.front().y];
    std::shared_ptr<Node> end = grid[food->x][food->y];

    for (size_t i = 0; i < grid_width; ++i) {
        for (size_t j = 0; j < grid_height; ++j) {
            auto node = grid[i][j];
            auto rect = sf::RectangleShape(sf::Vector2f(grid_size, grid_size));
            rect.setPosition(node->x * grid_size, node->y * grid_size);

            int indexInSnake = -1;
            for (size_t k = 0; k < snake.body.size(); ++k) {
                if (snake.body[k].x == node->x && snake.body[k].y == node->y) {
                    indexInSnake = k;
                    break;
                }
            }
            if (indexInSnake != -1) {
                rect.setFillColor(sf::Color(235, 52, 116, 255));
            }
            else if (node->isWall) {
                rect.setFillColor(sf::Color(50, 50, 50, 255));
            }
            else if (node == end) {
                rect.setFillColor(sf::Color(187, 134, 252));
            }
            else if (node->isPath) {
                rect.setFillColor(sf::Color(235, 52, 116, 50));
            }
            else {
                rect.setFillColor(sf::Color::Black);
            }
            window.draw(rect);
        }
    }
}

std::shared_ptr<GameState> GameScreen::Run() {

    sf::Clock clock;
    unsigned int in_game_time = 0;
    unsigned int current_steps = 0;
    int score = 0;

    //count

    // load audio file from assets
    sf::SoundBuffer buffer;
    buffer.loadFromFile("assets/audio/shady.mp3");
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setLoop(true);
    sound.play();

    while (window.isOpen()) {
        auto event = sf::Event{};
        sf::Time elapsed = clock.getElapsedTime();
        if (elapsed.asSeconds() > 0.25f) {
            current_steps++;

            const unsigned int grid_width = window.getSize().x / grid_size;
            const unsigned int grid_height = window.getSize().y / grid_size;

            auto reset_food = [&]() {
                food->x = std::rand() % grid_width;
                food->y = std::rand() % grid_height;
                score += (shortest_distance - current_steps);
                current_steps = 0;
                // reset the grid for a-star
                for (size_t i = 0; i < grid.size(); ++i) {
                    for (size_t j = 0; j < grid[i].size(); ++j) {
                        grid[i][j]->Reset();
                    }
                }
                SolveAStar(grid[snake.body.front().x][snake.body.front().y], grid[food->x][food->y]);

                // calculate the shortest distance
                shortest_distance = 0;
                for (size_t i = 0; i < grid.size(); ++i) {
                    for (size_t j = 0; j < grid[i].size(); ++j) {
                        if (grid[i][j]->isPath) {
                            shortest_distance++;
                        }
                    }
                }
            };

            bool game_over = false;
            snake.Update(reset_food, game_over);

            clock.restart();

            if (++in_game_time >= 12 * 60) {
                game_over = true;
            }

            if (game_over) {
                sound.stop();
                return std::make_shared<GameOver>(window, score, in_game_time);
            }
        }

        sf::Font font;
        font.loadFromFile("assets/fonts/Sharp Retro/Sharp Retro.ttf");

        sf::Text in_game_clock;
        in_game_clock.setFont(font);
        std::string time = get_formatted_time(in_game_time);
        in_game_clock.setString(time);
        in_game_clock.setCharacterSize(32);
        in_game_clock.setPosition(8, 8);

        sf::Text shortest_distance_text;
        shortest_distance_text.setFont(font);
        shortest_distance_text.setString("Shortest distance: " + std::to_string(shortest_distance));
        shortest_distance_text.setCharacterSize(32);
        shortest_distance_text.setPosition(window.getSize().x - 400, 8);

        sf::Text current_steps_text;
        current_steps_text.setFont(font);
        current_steps_text.setString("Current steps: " + std::to_string(current_steps));
        current_steps_text.setCharacterSize(32);
        current_steps_text.setPosition(window.getSize().x - 400, 48);

        sf::Text score_text;
        score_text.setFont(font);
        score_text.setString("Score: " + std::to_string(score));
        score_text.setCharacterSize(32);
        score_text.setPosition(window.getSize().x - 400, window.getSize().y - 88);

        sf::Text alert_score_delta;
        alert_score_delta.setFont(font);
        int delta = static_cast<int>(shortest_distance) - static_cast<int>(current_steps);
        std::string delta_string = std::to_string(delta);
        if (delta > 0) {
            alert_score_delta.setFillColor(sf::Color::Green);
            delta_string = "+" + delta_string;
        }
        else {
            alert_score_delta.setFillColor(sf::Color::Red);
        }
        alert_score_delta.setString(delta_string);
        alert_score_delta.setCharacterSize(32);
        // set at bottom right
        alert_score_delta.setPosition(window.getSize().x - 400, window.getSize().y - 48);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    return std::make_shared<TitleScreen>(window);
                }
                if (event.key.code == sf::Keyboard::Up) {
                    snake.velocity = sf::Vector2i(0, -1);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    snake.velocity = sf::Vector2i(0, 1);
                }
                if (event.key.code == sf::Keyboard::Left) {
                    snake.velocity = sf::Vector2i(-1, 0);
                }
                if (event.key.code == sf::Keyboard::Right) {
                    snake.velocity = sf::Vector2i(1, 0);
                }
            }
        }
        window.clear();
        Draw();
        window.draw(in_game_clock);
        window.draw(shortest_distance_text);
        window.draw(current_steps_text);
        window.draw(score_text);
        window.draw(alert_score_delta);
        window.display();
    }

    sound.stop();
    return nullptr;
}

void GameScreen::SolveAStar(std::shared_ptr<Node> start, std::shared_ptr<Node> end) {
    auto distance = [&](std::shared_ptr<Node> a, std::shared_ptr<Node> b) {
        //return std::abs(static_cast<int>(a->x) - static_cast<int>(b->x)) + std::abs(static_cast<int>(a->y) - static_cast<int>(b->y));
        // manhattan distance with modulus
        return std::abs(static_cast<int>(a->x) - static_cast<int>(b->x)) % grid_size + std::abs(static_cast<int>(a->y) - static_cast<int>(b->y)) % grid_size;
    };

    auto heuristic = [distance](std::shared_ptr<Node> a, std::shared_ptr<Node> b) {
        return distance(a, b);
    };

    std::shared_ptr<Node> current = start;
    start->localGoal = 0.0f;
    start->globalGoal = heuristic(start, end);

    std::list<std::shared_ptr<Node>> openList;
    openList.push_back(start);

    while (!openList.empty()) {
        openList.sort([](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->globalGoal < b->globalGoal;
            });

        while (!openList.empty() && openList.front()->isVisited) {
            openList.pop_front();
        }

        if (openList.empty()) {
            break;
        }

        current = openList.front();
        current->isVisited = true;

        for (auto neighbour : current->neighbours) {

            bool isSnake = false;
            for (auto& segment : snake.body) {
                if (segment.x == neighbour->x && segment.y == neighbour->y) {
                    isSnake = true;
                    break;
                }
            }

            if (!neighbour->isVisited && !neighbour->isWall && !isSnake) {
                float possiblyLowerGoal = current->localGoal + distance(current, neighbour);

                if (possiblyLowerGoal < neighbour->localGoal) {
                    neighbour->parent = current;
                    neighbour->localGoal = possiblyLowerGoal;
                    neighbour->globalGoal = neighbour->localGoal + heuristic(neighbour, end);

                    if (std::find(openList.begin(), openList.end(), neighbour) == openList.end()) {
                        openList.push_back(neighbour);
                    }
                }
            }
        }

        if (current == end) {
            while (current != start) {
                current->isPath = true;
                current = current->parent;
            }
            break;
        }
    }
}
