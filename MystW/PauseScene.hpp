#pragma once
#include "Scene.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class PauseScene : public Scene {
private:
    int selectedItemIndex;
    sf::Font font;
    std::vector<sf::Text> menuItems;

public:
    PauseScene(SceneManager* manager);

    void handleEvent(sf::Event event) override;
    void update(sf::Time delta) override;
    void render(sf::RenderWindow& window) override;

    void moveUp();
    void moveDown();
    int getSelectedIndex() const;
};