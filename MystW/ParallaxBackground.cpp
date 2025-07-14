#include "ParallaxBackground.hpp"
#include <filesystem>
#include <iostream>

BackgroundLayer::BackgroundLayer(float speed) : scrollSpeed(speed) {}

void BackgroundLayer::updateMoveRight(float delta) {
    //Scrolling
    sprite1.move(-scrollSpeed * delta, 0.0f);
    sprite2.move(-scrollSpeed * delta, 0.0f);
    sprite3.move(-scrollSpeed * delta, 0.0f);

    float width = sprite1.getGlobalBounds().width;
    float totalWidth = width * 3.0f;

    // Move forward a distance equal to the half of total width when background go out the screen
    if (sprite1.getPosition().x <= -width)
        sprite1.move(totalWidth, 0.0f);
    if (sprite2.getPosition().x <= -width)
        sprite2.move(totalWidth, 0.0f);
    if (sprite3.getPosition().x <= -width)
        sprite3.move(totalWidth, 0.0f);
}

void BackgroundLayer::updateMoveLeft(float delta) {
    //Scrolling
    sprite1.move(scrollSpeed * delta, 0.0f);
    sprite2.move(scrollSpeed * delta, 0.0f);
    sprite3.move(scrollSpeed * delta, 0.0f);

    float width = sprite1.getGlobalBounds().width;
    float totalWidth = width * 3.0f;

    // Move forward a distance equal to the half of total width when background go out the screen
    if (sprite1.getPosition().x >= width)
        sprite1.move(-totalWidth, 0.0f);
    if (sprite2.getPosition().x >= width)
        sprite2.move(-totalWidth, 0.0f);
    if (sprite3.getPosition().x >= width)
        sprite3.move(-totalWidth, 0.0f);

}

void BackgroundLayer::draw(sf::RenderWindow& window) {
    window.draw(sprite1);
    window.draw(sprite2);
    window.draw(sprite3);
}

void BackgroundLayer::load(const std::string& path, sf::RenderWindow& window)
{
    if (texture.loadFromFile(path))
    {
        sf::Vector2u windowSize = window.getSize();
        sf::Vector2u textureSize = texture.getSize();
        float scaleX = static_cast<float>(windowSize.x) / static_cast<float>(textureSize.x);
        float scaleY = static_cast<float>(windowSize.y) / static_cast<float>(textureSize.y);
        std::cout << "Loaded: " << path << std::endl;

        sprite1.setTexture(texture);
        sprite2.setTexture(texture);
        sprite3.setTexture(texture);

        sprite1.setScale(scaleX, scaleY);
        sprite2.setScale(scaleX, scaleY);
        sprite3.setScale(scaleX, scaleY);

        float scaleWidth = sprite1.getGlobalBounds().width;

        sprite2.setPosition(scaleWidth, 0.0f);
        sprite3.setPosition(scaleWidth * 2.0f, 0.0f);

    }
    else
        std::cout << "Cannot load file \n";
}

void ManageLayer::loadStage(const std::string& folderPath, sf::RenderWindow& window)
{
    layers.clear();
    float baseSpeed = 50.0f;
    int layerIndex = 0;
    std::list<std::string> imagePath;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
    {
        if (!entry.is_regular_file())
            continue;
        std::string path = entry.path().string();
        if (path.substr(path.size() - 4) == ".png" || path.substr(path.size() - 4) == ".jpg")
        {
            std::cout << "Take path successfully \n";
            imagePath.push_back(path);
        }
    }
    imagePath.sort();

    for (const auto& path : imagePath)
    {
        layers.emplace_back(baseSpeed + (layerIndex * 20.0f));
        layers.back().load(path, window);
        layerIndex++;
    }
}

void ManageLayer::update(float delta, char pressButton)
{
    for (auto& layer : layers)
    {
        if (pressButton == 'A')
            layer.updateMoveLeft(delta);
        else if (pressButton == 'D')
            layer.updateMoveRight(delta);
    }
}

void ManageLayer::draw(sf::RenderWindow& window)
{
    for (auto& layer : layers)
        layer.draw(window);
}