#include <iostream>
#include <vector>
#include <list>
#include <numeric>
#include <cstdlib> // for rand
#include <cmath>
#include <algorithm>
#include <memory>

#include "node.hpp"
#include "game_screen.hpp"
#include "title_screen.hpp"
#include "snake.hpp"

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

    this->snake = Snake(grid_width, grid_height, food);

    std::shared_ptr<Node> start = grid[snake.body.front().x][snake.body.front().y];
    std::shared_ptr<Node> end = grid[food->x][food->y];

    SolveAStar(start, end);
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
                rect.setFillColor(sf::Color::Red);
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
    while (window.isOpen()) {
        auto event = sf::Event{};

        sf::Time elapsed = clock.getElapsedTime();
        if (elapsed.asSeconds() > 0.25f) {
            const unsigned int grid_width = window.getSize().x / grid_size;
            const unsigned int grid_height = window.getSize().y / grid_size;

            auto reset_food = [&]() {food->x = std::rand() % grid_width; food->y = std::rand() % grid_height;};

            snake.Update(reset_food);
            //clear the path
            for (size_t i = 0; i < grid.size(); ++i) {
                for (size_t j = 0; j < grid[i].size(); ++j) {
                    grid[i][j]->Reset();
                }
            }
            SolveAStar(grid[snake.body.back().x][snake.body.back().y], grid[food->x][food->y]);
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
