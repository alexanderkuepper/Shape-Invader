//
// Created by Alex on 02.01.2024.
//

#include <memory>
#include <iostream>
#include "Game.h"

Game::Game() {
    window.create(sf::VideoMode{width, height}, "Shape Invader");
    window.setFramerateLimit(100);
}

void Game::update() {
    deltaTime = clock.getElapsedTime().asSeconds();
    clock.restart();
    switch (gameState) {
        case GameState::gameMenueScreen:
            updateMenueScreen();
            break;
        case GameState::gamePlay:
            updateGamePlay();
            break;
        case GameState::gameOverScreen:
            updateGameOverScreen();
            break;
    }
    checkCloseButton();
}

void Game::draw() {
    window.clear();
    switch (gameState) {
        case GameState::gameMenueScreen:
            drawMenueScreen();
            break;
        case GameState::gamePlay:
            drawGamePlay();
            break;
        case GameState::gameOverScreen:
            drawGameOverScreen();
            break;
    }
    window.display();
}

void Game::checkCloseButton() {
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            quitGame();
        }
    }
}

void Game::generateAsteroid() {
    if (asteroidClock.getElapsedTime().asSeconds() > 0.1) {
        asteroids.push_back(std::make_unique<Asteroid>());
        asteroidClock.restart();
    }
}

void Game::clearOffscreenAsteroids() {
    asteroids.erase(
            std::remove_if(asteroids.begin(), asteroids.end(), [](auto &asteroid) {
                return asteroid->position.y > height;
            }), asteroids.end()
    );
}

void Game::clearOffscreenBullets() {
    bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(), [](auto &bullet) {
                return bullet->position.y < 0;
            }), bullets.end()
    );
}


void Game::quitGame() {
    window.close();
    isRunning = false;
}

void Game::updateGamePlay() {
    player.update(deltaTime);
    generateAsteroid();
    clearOffscreenAsteroids();
    clearOffscreenBullets();
    if(InputManager::get().shoot()) {
        shootBullet();
    }
    for (auto &asteroid: asteroids) {
        asteroid->update(deltaTime);
        if (player.collisionCheck(*asteroid)) {
            gameState = GameState::gameOverScreen;
            asteroids.clear();
            bullets.clear();
            player.setPlayerStartPosition();
        }
    }
    for (auto &bullet : bullets) {
        bullet->update(deltaTime);
    }

    // Todo delete asteroids if collision detected with bullet
    /*auto asteroidIter = asteroids.begin();
    auto bulletIter = bullets.begin();
    while(asteroidIter != asteroids.end()) {
        while(bulletIter != bullets.end()) {
            if((*bulletIter)->collisionCheck(**asteroidIter)) {
                bullets.erase(bulletIter);
                asteroids.erase(asteroidIter);
                return;
            }
            bulletIter++;
        }
      asteroidIter++;
    }*/

    asteroids.erase(
            std::remove_if(asteroids.begin(), asteroids.end(), [this](auto &asteroid) {
                for(auto &bullet : bullets) {
                    if(asteroid->collisionCheck(*bullet)) {
                        return true;
                    }
                }
                return false;
            }), asteroids.end()
    );
}

void Game::updateMenueScreen() {

}

void Game::updateGameOverScreen() {
    if(InputManager::restart()) {
        gameState = GameState::gamePlay;
    }
}

void Game::drawGamePlay() {
    player.draw(window);
    for (auto &asteroid: asteroids) {
        asteroid->draw(window);
    }
    for (auto &bullet : bullets) {
        bullet->draw(window);
    }
}

void Game::drawMenueScreen() {

}

void Game::drawGameOverScreen() {

}

void Game::shootBullet() {
    bullets.push_back(std::make_unique<Bullet>(getShootPosition()));
}

sf::Vector2f Game::getShootPosition() {
    return sf::Vector2f{player.position.x + player.sprite.getGlobalBounds().width / 2,player.position.y};
}
