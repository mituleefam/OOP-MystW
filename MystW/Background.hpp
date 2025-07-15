#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
class BackgroundLayer {
private:
    sf::Texture texture;
    sf::Sprite sprite1, sprite2, sprite3;
    float scrollSpeed = 2.0f;
public:
    BackgroundLayer(float speed);
    void updateMoveLeft(float delta);
    void updateMoveRight(float delta);
    void draw(sf::RenderWindow& window);
    void load(const std::string& path, sf::RenderWindow& window);
};


class ManageLayer
{
private:
    std::list<BackgroundLayer> layers;
public:
    void loadStage(const std::string& folderPath, sf::RenderWindow& window);
    void update(float delta, char pressButton);
    void draw(sf::RenderWindow& window);
};

