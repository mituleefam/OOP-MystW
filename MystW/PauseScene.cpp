#include "PauseScene.hpp"
#include "SceneManager.hpp"
#include <iostream>

PauseScene::PauseScene(SceneManager* manager) : Scene(manager) {
    if (!font.loadFromFile("PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    std::vector<std::string> labels = { "Resume", "Main Menu", "Exit" };

    for (int i = 0; i < labels.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(labels[i]);
        text.setCharacterSize(50);
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
        text.setPosition(400.f, 300.f + i * 80.f);  // Centered
        menuItems.push_back(text);
    }

    selectedItemIndex = 0;
}

void PauseScene::handleEvent(sf::Event event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::Up:
            moveUp();
            break;
        case sf::Keyboard::Down:
            moveDown();
            break;
        case sf::Keyboard::Enter:
            switch (selectedItemIndex) {
            case 0: // Resume
                sceneManager->changeScene("Game");
                break;
            case 1: // Main Menu
                sceneManager->changeScene("Menu");
                break;
            case 2: // Exit
                exit(0);
                break;
            }
            break;
        default:
            break;
        }
    }
}

void PauseScene::update(sf::Time delta) {
    // No animation/update logic for now
}

void PauseScene::render(sf::RenderWindow& window) {
    for (const auto& item : menuItems)
        window.draw(item);
}

void PauseScene::moveUp() {
    if (selectedItemIndex > 0) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex--;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void PauseScene::moveDown() {
    if (selectedItemIndex < menuItems.size() - 1) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex++;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

int PauseScene::getSelectedIndex() const {
    return selectedItemIndex;
}