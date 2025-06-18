#include "SpiritBullet.h"

SpiritBullet::SpiritBullet(sf::Vector2f startPos, sf::Vector2f direction, float speed)
    : lifetime(5.0f) // Bullet will exist for a maximum of 5 seconds.
{
    shape.setSize(sf::Vector2f(12.0f, 12.0f));
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin(6.0f, 6.0f); // Center the origin for proper rotation/scaling if needed later
    shape.setPosition(startPos);
    velocity = direction * speed;
}

void SpiritBullet::update(float deltaTime) {
    shape.move(velocity * deltaTime);

    // Countdown the lifetime.
    lifetime -= deltaTime;
    if (lifetime <= 0) {
        isAlive = false;
    }
}

void SpiritBullet::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect SpiritBullet::getGlobalBounds() const {
    return shape.getGlobalBounds();
}
