#pragma once

#include "Scene.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class SceneManager;

class GameScene : public Scene {
public:
    GameScene(SceneManager* manager);
    ~GameScene();
    void handleEvent(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    SceneManager* sceneManager;
    sf::Font font;
    sf::Text debugText;
    //List of textures and sprites for each layer
    std::vector<sf::Texture> bgTextures;
    std::vector<std::pair<sf::Sprite, sf::Sprite>> bgSprites; // Each layer has 2 sprite to scroll
    std::vector<float> scrollSpeeds; // Scroll speed for each layer
    float baseScrollSpeed = 200.f; // Basic speed (can modify)

    void loadBackgroundLayers(const std::string& folderPath);
};


