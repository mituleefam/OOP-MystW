// Camera.cpp
#include "Camera.h"

Camera::Camera(const sf::Vector2f& winSize, const sf::Vector2f& mapSize)
    : windowSize(winSize), mapSize(mapSize) {
    view.setSize(windowSize);
    view.setCenter(windowSize.x / 2, windowSize.y / 2);
}

void Camera::follow(const sf::Vector2f& targetPos) {
    float halfW = view.getSize().x / 2;
    float halfH = view.getSize().y / 2;

    float x = std::max(halfW, std::min(targetPos.x, mapSize.x - halfW));
    float y = std::max(halfH, std::min(targetPos.y, mapSize.y - halfH));

    view.setCenter(x, y);
}

void Camera::applyTo(sf::RenderWindow& window) {
    window.setView(view);
}
