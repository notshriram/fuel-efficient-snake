#include <iostream>
#include <vector>
#include <list>
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "node.hpp"
#include "game_screen.hpp"
#include "title_screen.hpp"
#include "snake.hpp"

GameScreen::GameScreen(sf::RenderWindow& window):
    GameState(window),
    grid(0),
    snake(Snake(sf::Vector2i{}, sf::Vector2i{})) {
    Init();
}

void GameScreen::Init()
{
    // initialize the game grid
    const unsigned int grid_size = 40; // pixels
    const unsigned int grid_width = window.getSize().x / grid_size;
    const unsigned int grid_height = window.getSize().y / grid_size;

    this->grid = std::vector<std::vector<std::shared_ptr<Node>>>(grid_width, std::vector<std::shared_ptr<Node>>(grid_height));

    for (unsigned int i = 0; i < grid_width; ++i) {
        for (unsigned int j = 0; j < grid_height; ++j) {
            grid[i][j] = std::make_shared<Node>();
            grid[i][j]->x = i;
            grid[i][j]->y = j;

            if (i == 0 || i == grid_width - 1 || j == 0 || j == grid_height - 1) {
                grid[i][j]->isWall = true;
            }
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
        }
    }

    std::shared_ptr<Node> start = grid[5][10];
    std::shared_ptr<Node> end = grid[12][10];

    this->snake = Snake(sf::Vector2i{5, 10}, sf::Vector2i{6, 10});
    SolveAStar(start, end);

}

void GameScreen::Draw() {
    const unsigned int grid_size = 40; // pixels
    const unsigned int grid_width = window.getSize().x / grid_size;
    const unsigned int grid_height = window.getSize().y / grid_size;

    std::shared_ptr<Node> start = grid[5][10];
    std::shared_ptr<Node> end = grid[12][10];

    for (size_t i = 0; i < grid_width; ++i) {
        for (size_t j = 0; j < grid_height; ++j) {
            auto node = grid[i][j];
            auto rect = sf::RectangleShape(sf::Vector2f(grid_size, grid_size));
            rect.setPosition(node->x * grid_size, node->y * grid_size);

            bool isSnake = false;
            for (auto& segment : snake.body) {
                if (segment.x == node->x && segment.y == node->y) {
                    isSnake = true;
                    break;
                }
            }
            if (isSnake) {
                rect.setFillColor(sf::Color::Magenta);
            }
            else if (node->isWall) {
                rect.setFillColor(sf::Color::Black);
            }
            else if (node == start) {
                rect.setFillColor(sf::Color::Green);
            }
            else if (node == end) {
                rect.setFillColor(sf::Color::Red);
            }
            else if (node->isPath) {
                rect.setFillColor(sf::Color::Yellow);
            }
            else if (node->isVisited) {
                rect.setFillColor(sf::Color::Blue);
            }
            else {
                rect.setFillColor(sf::Color::White);
            }
            rect.setOutlineColor(sf::Color::Black);
            rect.setOutlineThickness(1.0f);
            window.draw(rect);
        }
    }
}

std::shared_ptr<GameState> GameScreen::Run() {

    sf::Clock clock;
    while (window.isOpen()) {
        auto event = sf::Event{};

        sf::Time elapsed = clock.getElapsedTime();
        if (elapsed.asSeconds() > 1.f) {
            snake.Update();
            clock.restart();
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
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
        window.display();
    }

    return nullptr;
}

void GameScreen::SolveAStar(std::shared_ptr<Node> start, std::shared_ptr<Node> end) {
    auto distance = [](std::shared_ptr<Node> a, std::shared_ptr<Node> b) {
        return std::sqrtf(std::pow(a->x - b->x, 2) + std::pow(a->y - b->y, 2));
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
