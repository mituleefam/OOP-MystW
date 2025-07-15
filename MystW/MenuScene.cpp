#include "MenuScene.hpp"
#include "SceneManager.hpp"
#include <iostream>

MenuScene::MenuScene(SceneManager* manager) : Scene(manager) {
    if (!font.loadFromFile("PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    std::vector<std::string> labels = { "Play", "Settings", "Exit" };

    for (int i = 0; i < labels.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(labels[i]);
        text.setCharacterSize(60);
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
        text.setPosition(400.f, 300.f + i * 100.f);  // Centered
        menuItems.push_back(text);
    }

    selectedItemIndex = 0;
}

void MenuScene::handleEvent(sf::Event event) {
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
            case 0: // Play
                if (sceneManager)
                    sceneManager->changeScene("Game");
                break;
            case 1: // Settings
                std::cout << "Settings selected (chưa xử lý)" << std::endl;
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

void MenuScene::update(sf::Time delta) {
    // No animation/update logic for now
}

void MenuScene::render(sf::RenderWindow& window) {
    for (const auto& item : menuItems)
        window.draw(item);
}

void MenuScene::moveUp() {
    if (selectedItemIndex > 0) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex--;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void MenuScene::moveDown() {
    if (selectedItemIndex < menuItems.size() - 1) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex++;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

int MenuScene::getSelectedIndex() const {
    return selectedItemIndex;
}