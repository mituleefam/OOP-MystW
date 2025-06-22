#pragma once
#include <SFML/Graphics.hpp>

class SceneManager;  // Using forward declaration to avoid loop

class Scene {
public:
    Scene(SceneManager* manager) : sceneManager(manager) {}
    virtual ~Scene() {}

    virtual void handleEvent(sf::Event event) = 0;
    virtual void update(sf::Time delta) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

protected:
    SceneManager* sceneManager;
};

