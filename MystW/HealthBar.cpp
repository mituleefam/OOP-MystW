#include "HealthBar.h"

HealthBar::HealthBar(float maxHealth, sf::Vector2f position, sf::Vector2f size, sf::Font* font)
    : maxHealth(maxHealth), currentHealth(maxHealth), position(position), size(size), font(font)
{
    background.setSize(size);
    background.setFillColor(sf::Color(100, 100, 100));
    background.setPosition(position);

    bar.setSize(size);
    bar.setFillColor(sf::Color::Green);
    bar.setPosition(sf::Vector2(position.x + 100, position.y));
    healthText.setFont(*this->font);
    healthText.setCharacterSize(size.y * 0.7f);
    healthText.setFillColor(sf::Color::White);
    healthText.setCharacterSize(20);

    setHealth(maxHealth);
}

void HealthBar::setPosition(const sf::Vector2f& newPos) {
    position = newPos;
    background.setPosition(position);
    bar.setPosition(position);

    // Cập nhật lại vị trí của text
    if (font) {
        sf::FloatRect textRect = healthText.getLocalBounds();
        healthText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        // Căn giữa text theo vị trí mới
        healthText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
    }
}

void HealthBar::setHealth(float health) {
    currentHealth = std::max(0.f, std::min(health, maxHealth));
    float healthPercent = currentHealth / maxHealth;
    bar.setSize(sf::Vector2f(size.x * healthPercent, size.y));
    if (healthPercent > 0.5f)
        bar.setFillColor(sf::Color::Green);
    else if (healthPercent > 0.25f)
        bar.setFillColor(sf::Color::Yellow);
    else
        bar.setFillColor(sf::Color::Red);
    std::string text = std::to_string((int)currentHealth) + " / " + std::to_string((int)maxHealth);
    healthText.setString(text);

    sf::FloatRect textRect = healthText.getLocalBounds();
    healthText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    healthText.setPosition((position.x + size.x / 2.0f) - 50, position.y + size.y / 2.0f);
}

void HealthBar::update(float newHealth) {
    setHealth(newHealth);
}

void HealthBar::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(bar);
    window.draw(healthText);
}

float HealthBar::getCurrentHealth()
{
    return currentHealth;
}

void HealthBar::setCurrentHealth(float health)
{
    currentHealth = health;
    setHealth(currentHealth);
}

sf::Vector2f HealthBar::getSize()
{
    return size;
}
