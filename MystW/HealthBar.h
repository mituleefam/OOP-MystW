#pragma once
#include <SFML/Graphics.hpp>

class HealthBar {
private:
    sf::RectangleShape background;
    sf::RectangleShape bar;

    float maxHealth;
    float currentHealth;

    sf::Vector2f position;
    sf::Vector2f size;
    sf::Text healthText;
    sf::Font* font;

public:
    HealthBar(float maxHealth, sf::Vector2f position, sf::Vector2f size, sf::Font* font);
    void setPosition(const sf::Vector2f& newPos);
    void setHealth(float health);
    void update(float newHealth);
    void draw(sf::RenderWindow& window);
    float getCurrentHealth();
    void setCurrentHealth(float health);
    sf::Vector2f getSize();
};
