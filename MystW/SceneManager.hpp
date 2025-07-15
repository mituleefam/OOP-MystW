#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <SFML/Graphics.hpp>

#include "Scene.hpp"

class SceneManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> currentScene;

public:
    void addScene(const std::string& name, std::shared_ptr<Scene> scene);
    void changeScene(const std::string& name);
    void handleEvent(sf::Event event);
    void update(sf::Time delta);
    void render(sf::RenderWindow& window);
};
