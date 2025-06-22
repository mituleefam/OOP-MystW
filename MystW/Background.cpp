#include "Background.hpp"
#include <filesystem>
#include <iostream>

BackgroundLayer::BackgroundLayer(const sf::Texture& texture, float scrollSpeed) {
    sprite1.setTexture(texture);
    sprite2.setTexture(texture);
    sprite3.setTexture(texture);
    sprite2.setPosition(static_cast<float>(texture.getSize().x), 0.0f);
    sprite3.setPosition(static_cast<float>(texture.getSize().x) * 2.0f, 0.0f);
    speed = scrollSpeed;
}

void BackgroundLayer::update() {
    sprite1.move(-speed, 0.0f);
    sprite2.move(-speed, 0.0f);
    sprite3.move(-speed, 0.0f);

    float width = static_cast<float>(sprite1.getTexture()->getSize().x);
    float totalWidth = width * 3.0f; // Total width of 3 sprites


    if (sprite1.getPosition().x <= -width)
        sprite1.move(totalWidth, 0.0f); // Move forward a distance equal to the total width
    if (sprite2.getPosition().x <= -width)
        sprite2.move(totalWidth, 0.0f);
    if (sprite3.getPosition().x <= -width)
        sprite3.move(totalWidth, 0.0f);
}

void BackgroundLayer::draw(sf::RenderWindow& window) {
    window.draw(sprite1);
    window.draw(sprite2);
    window.draw(sprite3);
}

bool BackgroundManager::loadStage(const std::string& folderPath, const std::vector<float>& speeds) {
    textures.clear();
    layers.clear();

    int i = 0;
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        sf::Texture tex;
        if (!tex.loadFromFile(entry.path().string())) {
            std::cout << "Failed to load " << entry.path() << '\n';
            return false;
        }
        textures.push_back(tex);
        if (i < speeds.size())
            layers.emplace_back(textures.back(), speeds[i]);
        i++;
    }
    return true;
}

void BackgroundManager::update() {
    for (auto& layer : layers)
        layer.update();
}

void BackgroundManager::draw(sf::RenderWindow& window) {
    for (auto& layer : layers)
        layer.draw(window);
}
