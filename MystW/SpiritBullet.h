#pragma once
#include <SFML/Graphics.hpp>

/**
 * @class SpiritBullet
 * @brief Represents a single projectile fired by the Spirit.
 * It manages its own movement and lifetime.
 */
class SpiritBullet {
public:
    /**
     * @brief Constructs a new Spirit Bullet.
     * @param startPos The initial position of the bullet.
     * @param direction The normalized direction vector for the bullet's movement.
     * @param speed The speed at which the bullet travels.
     */
    SpiritBullet(sf::Vector2f startPos, sf::Vector2f direction, float speed);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getGlobalBounds() const;
	sf::Vector2f getPosition() const;

    // Public flag to check if the bullet should be removed.
    bool isAlive = true;

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime; // To remove bullets that fly too far or exist for too long.
};
