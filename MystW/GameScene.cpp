#include "GameScene.hpp"
#include "SceneManager.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

GameScene::GameScene(SceneManager* manager) : Scene(manager), sceneManager(manager) {
    std::cout << "Initializing GameScene\n";
    if (!font.loadFromFile("cursive.ttf")) {
        std::cerr << "Failed to load font\n";
        throw std::runtime_error("Font loading failed");
    }
    std::cout << "Font loaded\n";

    debugText.setFont(font);
    debugText.setString("Game Scene");
    debugText.setCharacterSize(36);
    debugText.setFillColor(sf::Color::White);
    sf::FloatRect bounds = debugText.getLocalBounds();
    debugText.setOrigin(bounds.width / 2, bounds.height / 2);
    debugText.setPosition(400.f, 300.f);

    std::cout << "Loading background layers\n";
    loadBackgroundLayers("resources");
    std::cout << "Loaded " << bgTextures.size() << " layers\n";
}

GameScene::~GameScene() {}

void GameScene::loadBackgroundLayers(const std::string& folderPath) {
    bgTextures.clear();
    bgSprites.clear();
    scrollSpeeds.clear();

    if (!fs::exists(folderPath)) {
        std::cerr << "Folder " << folderPath << " does not exist\n";
        return;
    }

    int layerIndex = 0;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg") {
            std::cout << "Loading " << entry.path().string() << "\n";
            sf::Texture texture;
            if (!texture.loadFromFile(entry.path().string())) {
                std::cerr << "Failed to load " << entry.path().string() << "\n";
                continue;
            }
            bgTextures.push_back(texture);

            sf::Sprite sprite1, sprite2;
            sprite1.setTexture(bgTextures.back());
            sprite2.setTexture(bgTextures.back());

            float scaleX = 800.f / static_cast<float>(bgTextures.back().getSize().x); // Width ratio
            float scaleY = 600.f / static_cast<float>(bgTextures.back().getSize().y); // Height ratio
            sprite1.setScale(scaleX, scaleY);
            sprite2.setScale(scaleX, scaleY);

            sprite2.setPosition(sprite1.getGlobalBounds().width, 0);

            bgSprites.emplace_back(sprite1, sprite2);

            float speedFactor = 2.5f / (layerIndex + 1);
            scrollSpeeds.push_back(baseScrollSpeed * speedFactor);
            layerIndex++;
        }
    }

    if (bgTextures.empty()) {
        std::cerr << "No background layers loaded\n";
    }
}

void GameScene::handleEvent(sf::Event event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        if (!sceneManager) {
            std::cerr << "SceneManager is null\n";
            return;
        }
        std::cout << "Escape pressed - returning to Menu\n";
        sceneManager->changeScene("Menu");
    }
}

void GameScene::update(sf::Time delta) {
    if (bgTextures.empty() || bgTextures.size() != bgSprites.size() || bgTextures.size() != scrollSpeeds.size()) {
        std::cerr << "Invalid background data\n";
        return;
    }

    for (size_t i = 0; i < bgSprites.size(); ++i) {
        float dx = scrollSpeeds[i] * delta.asSeconds();
        bgSprites[i].first.move(-dx, 0);
        bgSprites[i].second.move(-dx, 0);

        float spriteWidth = bgSprites[i].first.getGlobalBounds().width;
        if (bgSprites[i].first.getPosition().x + spriteWidth < 0) {
            bgSprites[i].first.setPosition(bgSprites[i].second.getPosition().x + spriteWidth, 0);
        }
        if (bgSprites[i].second.getPosition().x + spriteWidth < 0) {
            bgSprites[i].second.setPosition(bgSprites[i].first.getPosition().x + spriteWidth, 0);
        }
    }
}

void GameScene::render(sf::RenderWindow& window) {
    if (bgTextures.empty()) {
        std::cerr << "No backgrounds to render\n";
        return;
    }

    for (const auto& spritePair : bgSprites) {
        window.draw(spritePair.first);
        window.draw(spritePair.second);
    }
}