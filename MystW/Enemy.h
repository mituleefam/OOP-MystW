#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Animation.h" // Assuming Animation.h contains your Animation struct
#include "AnimatedProjectile.h" // For potential common projectile management later

// Enum for general enemy states
enum class EnemyState { Idle, Running, Attacking, Hurt, Dying };

class Enemy {
public:
    // Constructor: takes starting position
    Enemy(float startX, float startY, int initialHealth, float enemySpeed);
    // Virtual destructor is crucial for polymorphism
    virtual ~Enemy() = default;

    // Pure virtual methods that MUST be implemented by derived classes
    virtual void loadSpecificAssets() = 0;
    //virtual void performAttackLogic(const sf::Vector2f& playerPos) = 0;
    virtual void performAttackLogic(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) = 0;
    //virtual void updateAI(float deltaTime, const sf::Vector2f& playerPos) = 0; 
    virtual void updateAI(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) = 0; // For specific movement/decision AI

    // Virtual methods that can be overridden (or used as is if base implementation is sufficient)
    virtual void update(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox);
    virtual void draw(sf::RenderWindow& window);
    virtual void animate(float deltaTime); // Animation logic might be common

    // Common methods
    void takeDamage(int damageAmount);
    bool isDead() const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;      // Gets the global bounds of the sprite
    sf::FloatRect getHitBox() const;      // Gets a more precise hitbox
    bool checkIsHurting() const;

protected:
    // Common protected members accessible by derived classes
    sf::Sprite sprite;
    sf::Vector2f position;
    float speed;
    int health;
    const int maxHealth; // Max health for this enemy type, set by derived class

    EnemyState currentState;
    std::map<EnemyState, Animation> animations; // Loaded by derived class
    int currentFrame;
    float animationTimer; // Renamed from elapsedTime for clarity within animation context
    float direction; // 1.0f for right, -1.0f for left

    bool isAlive;
    bool isHurting;
    sf::Clock hurtTimer;
    float hurtDuration; // Duration of the hurt state visual/invulnerability

    // Helper to change state and reset animation
    void setState(EnemyState newState);
    void updateFacingDirection(const sf::Vector2f& playerPos);
    virtual void adjustSpriteOriginAndScale(); // For flipping sprite

    // Projectile related members - if many enemies shoot, could be here.
    // For now, keeping projectiles specific to Elf.
    // std::vector<AnimatedProjectile> activeProjectiles;
};