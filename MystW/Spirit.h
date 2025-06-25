#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Enemy.h"        // Needs the base Enemy definition for targeting.
#include "SpiritBullet.h" // Uses the new bullet class.

/**
 * @class Spirit
 * @brief A companion that follows the player and automatically attacks enemies.
 * It remains inactive until explicitly activated.
 */
class Spirit {
public:
    Spirit();

    // --- Public Methods ---

    /**
     * @brief Loads all necessary visual assets for the spirit.
     * Should be called once during game initialization.
     */
    void loadAssets();

    /**
     * @brief Activates the spirit, allowing it to update and draw.
     * This is called when the player acquires the spirit in the game.
     */
    void activate();

    /**
     * @brief Checks if the spirit is currently active.
     * @return True if active, false otherwise.
     */
    bool isActive() const;

    /**
     * @brief The main update loop for the spirit.
     * Handles all logic including movement, animation, targeting, and attacking.
     * @param deltaTime Time elapsed since the last frame.
     * @param playerPosition The current position of the player for following.
     * @param enemies A vector of all active enemies in the game world.
     */
    void update(float deltaTime, const sf::Vector2f& playerPosition, std::vector<std::unique_ptr<Enemy>>& enemies);

    /**
     * @brief Draws the spirit and its active projectiles to the window.
     * @param window The RenderWindow to draw to.
     */
    void draw(sf::RenderWindow& window);

private:
    // --- Private Members ---

    // State Management
    bool bIsActive;

    // Visuals and Position
    sf::Sprite sprite;
    sf::Texture textureSheet;
    sf::IntRect animationRect;
    float animationTimer;

    // Combat
    std::vector<SpiritBullet> bullets;
    sf::Clock attackCooldown;
    float attackInterval;   // Time in seconds between attacks.
    float detectionRange;   // How far the spirit can "see" enemies.
    Enemy* currentTarget;   // A raw pointer to the current enemy target. Does not own the enemy.

    // --- Private Helper Methods ---

    /**
     * @brief Updates the spirit's animation frame.
     */
    void updateAnimation(float deltaTime);

    /**
     * @brief Updates the spirit's position to smoothly follow the player.
     */
    void updatePosition(const sf::Vector2f& playerPosition);

    /**
     * @brief Scans for the closest valid enemy within detection range.
     * @param enemies A vector of all active enemies.
     */
    void findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies);

    /**
     * @brief Handles the logic for firing a projectile at the current target.
     */
    void handleAttacking();
};
