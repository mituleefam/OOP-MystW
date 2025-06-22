#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
class BackgroundLayer {
public:
    sf::Sprite sprite1, sprite2, sprite3;
    float speed;

    BackgroundLayer(const sf::Texture& texture, float scrollSpeed);
    void update();
    void draw(sf::RenderWindow& window);
};

class BackgroundManager {
private:
    std::list<sf::Texture> textures;
    std::vector<BackgroundLayer> layers;
public:
    bool loadStage(const std::string& folderPath, const std::vector<float>& speeds);
    void update();
    void draw(sf::RenderWindow& window);
};
