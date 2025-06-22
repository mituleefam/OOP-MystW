#include "SceneManager.hpp"
#include <iostream>
void SceneManager::addScene(const std::string& name, std::shared_ptr<Scene> scene) {
    scenes[name] = scene;
}

void SceneManager::changeScene(const std::string& name) {
    if (scenes.count(name)) {
        currentScene = scenes[name];
    }
    else {
        std::cerr << "SceneManager: Scene \"" << name << "\" not found!\n";
    }
}

void SceneManager::handleEvent(sf::Event event) {
    if (currentScene) currentScene->handleEvent(event);
}

void SceneManager::update(sf::Time delta) {
    if (currentScene) currentScene->update(delta);
}

void SceneManager::render(sf::RenderWindow& window) {
    if (currentScene) currentScene->render(window);
}
