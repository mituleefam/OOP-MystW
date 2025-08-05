#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
class BackgroundLayer {
private:
    sf::Texture texture;
    sf::Sprite sprite; //1, sprite2, sprite3;
    // float scrollSpeed = 2.0f;
    float parallaxFactor;
    unsigned int textureWidth; // << Lưu lại chiều rộng của ảnh
    sf::RenderWindow* windowRef; // << Cần tham chiếu đến cửa sổ
public:
    BackgroundLayer(float factor);
    //void updateMoveLeft(float delta);
    //void updateMoveRight(float delta);
    void update(sf::Vector2f cameraPosition); // HÀM UPDATE MỚI
    void draw(sf::RenderWindow& window);
    void load(const std::string& path, sf::RenderWindow& window);
};


class ManageLayer
{
private:
    std::list<BackgroundLayer> layers;
public:
    void loadStage(const std::string& folderPath, sf::RenderWindow& window);
    //void update(float delta, char pressButton);
    void update(sf::Vector2f cameraPosition);
    void draw(sf::RenderWindow& window);
};
