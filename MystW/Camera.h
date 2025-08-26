#pragma once
#include <SFML/Graphics.hpp>

class Camera {
private:
    sf::View view;
    sf::Vector2f mapSize;
    sf::Vector2f windowSize;

public:
    Camera() {}
    Camera(const sf::Vector2f& winSize, const sf::Vector2f& mapSize);

    void follow(const sf::Vector2f& targetPos); // theo dõi player
    void applyTo(sf::RenderWindow& window);     // áp dụng camera
    sf::View getView() const { return view; }
};
