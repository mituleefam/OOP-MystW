#include "Spirit.h"
#include "Math.h"   // For normalizeVector and magnitude (assuming these exist)
#include <iostream>
#include <limits>   // For std::numeric_limits to get max float value

Spirit::Spirit()
    : bIsActive(false),       // Start as inactive.
    animationTimer(0.0f),
    attackInterval(0.8f),   // Fires one bullet every 0.8 seconds.
    detectionRange(800.0f), // Can detect enemies up to 800 pixels away.
    currentTarget(nullptr)
{
    animationRect = sf::IntRect(0, 0, 16, 24);
}

void Spirit::loadAssets() {
    if (textureSheet.loadFromFile("Assets/Spirit/Textures/meow01.png")) {
        sprite.setTexture(textureSheet);
        sprite.setTextureRect(animationRect);
        sprite.setScale(3.0f, 3.0f);
        sprite.setOrigin(8.0f, 12.0f); // Set origin to the center of the sprite.
        std::cout << "Spirit assets loaded successfully." << std::endl;
    }
    else {
        std::cerr << "Error loading spirit texture." << std::endl;
    }
}

void Spirit::activate() {
    bIsActive = true;
    std::cout << "Spirit has been activated!" << std::endl;
}

bool Spirit::isActive() const {
    return bIsActive;
}

void Spirit::update(float deltaTime, const sf::Vector2f& playerPosition, std::vector<std::unique_ptr<Enemy>>& enemies) {
    if (!bIsActive) {
        return; // Do nothing if not active.
    }

    // Always update position and animation.
    updatePosition(playerPosition);
    updateAnimation(deltaTime);

    // --- Bullet Management ---
    // 1. Update all active bullets.
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
    }

    // 2. Check for collisions between bullets and enemies.
    for (auto& bullet : bullets) {
        if (!bullet.isAlive) continue; // Skip bullets already marked for deletion.

        for (const auto& enemy : enemies) {
            if (enemy && !enemy->isDead() && bullet.getGlobalBounds().intersects(enemy->getHitBox())) {
                enemy->takeDamage(1);      // The spirit's damage.
                bullet.isAlive = false;    // Destroy the bullet on impact.
                break; // A single bullet can only hit one enemy.
            }
        }
    }

    // 3. Remove bullets that are no longer alive (hit an enemy or lifetime expired).
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const SpiritBullet& b) {
        return !b.isAlive;
        }), bullets.end());


    // --- AI and Targeting Logic ---
    // Always scan for the best target.
    findTarget(enemies);

    // If a target is found, handle the attack.
    if (currentTarget) {
        handleAttacking();
    }
}

void Spirit::draw(sf::RenderWindow& window) {
    if (!bIsActive) {
        return;
    }

    // Draw the spirit itself.
    window.draw(sprite);

    // Draw all its bullets.
    for (const auto& bullet : bullets) {
        bullet.draw(window);
    }
}

void Spirit::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer > 0.2f) {
        animationRect.top += 24;
        if (animationRect.top >= 144) { // Assumes 6 frames of 24px height.
            animationRect.top = 0;
        }
        sprite.setTextureRect(animationRect);
        animationTimer = 0.0f;
    }
}

void Spirit::updatePosition(const sf::Vector2f& playerPosition) {
    // Define the desired hover position relative to the player.
    sf::Vector2f targetPos = playerPosition + sf::Vector2f(-100.0f, -100.0f);

    // Smoothly move towards the target position using interpolation for a "floaty" effect.
    sf::Vector2f currentPos = sprite.getPosition();
    sprite.setPosition(currentPos + (targetPos - currentPos) * 0.1f);
}

void Spirit::findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    currentTarget = nullptr;
    float closestDistanceSq = std::numeric_limits<float>::max(); // Use max float value as initial closest distance.

    for (const auto& enemy : enemies) {
        // Ensure the enemy is valid and not already dying.
        if (enemy && !enemy->isDead()) {
            // Using squared distance is more efficient because it avoids a square root calculation.
            float distSq = magnitude(enemy->getPosition() - sprite.getPosition());
            distSq *= distSq;

            // Check if this enemy is within detection range AND is closer than the previous closest.
            if (distSq < detectionRange * detectionRange && distSq < closestDistanceSq) {
                closestDistanceSq = distSq;
                currentTarget = enemy.get(); // Set this enemy as the new target.
            }
        }
    }
}

void Spirit::handleAttacking() {
    // Only fire if a target exists and the attack is not on cooldown.
    if (currentTarget && attackCooldown.getElapsedTime().asSeconds() > attackInterval) {

        // Aim at the center of the target's hitbox for better accuracy.
        sf::FloatRect targetBounds = currentTarget->getHitBox();
        sf::Vector2f targetCenter(targetBounds.left + targetBounds.width / 2.f, targetBounds.top + targetBounds.height / 2.f);

        // Calculate the direction from the spirit to the target.
        sf::Vector2f direction = targetCenter - sprite.getPosition();
        normalizeVector(direction); // Ensure it's a unit vector.

        // Create a new bullet.
        bullets.emplace_back(sprite.getPosition(), direction, 700.0f);

        // Restart the cooldown timer.
        attackCooldown.restart();
    }
}
