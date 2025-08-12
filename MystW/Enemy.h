#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Animation.h" // Assuming Animation.h contains your Animation struct
#include "AnimatedProjectile.h" // For potential common projectile management later
#include "CollisionLayer.hpp"
#include "HealthBar.h"

class Player;
// Enum for general enemy states
enum class EnemyState {
    Idle,
    Running,
    Jumping,
    Attacking,  // Tấn công/Phép thuật chung
    Attacking2, // Cận chiến 2
    Attacking3,
    Attacking4,
    Attacking5,
    Hurt,
    Dying,
    Defending,
    Rolling,
    Healing
};

// This struct is now the standard for loading sprite sheet animations
struct AnimSheetInfo
{
    const char* filename;
    int frameWidth;
    int frameHeight;
    int frameCount;
    float frameDuration;
    bool loop;
};

class Enemy {
public:
    // Constructor: takes starting position
    Enemy(float startX, float startY, int initialHealth, float enemySpeed);
    // Virtual destructor is crucial for polymorphism
    virtual ~Enemy() = default;

    // Pure virtual methods that MUST be implemented by derived classes
    virtual void loadSpecificAssets() = 0;
    virtual void performAttackLogic(Player& player) = 0;
    virtual void updateAI(float deltaTime, Player& player) = 0;
    // Virtual methods that can be overridden (or used as is if base implementation is sufficient)
	virtual void update(float deltaTime, Player& player, const CollisionLayer& collisionLayer);
    virtual void draw(sf::RenderWindow& window);
    virtual void animate(float deltaTime); // Animation logic might be common

	virtual sf::FloatRect getHitBox() const;      // Gets a more precise hitbox depending on the enemy's sprite
    void handlePhysicsAndCollision(float deltaTime, const CollisionLayer& collisionLayer);

    // Common methods
    virtual void takeDamage(int damageAmount);
    bool isDead() const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;      // Gets the global bounds of the sprite
    bool checkIsHurting() const;
    EnemyState getCurrentState() const;

protected:
    // Common protected members accessible by derived classes
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::FloatRect localHitbox;
    // === THÊM CÁC BIẾN VẬT LÝ MỚI ===
    sf::Vector2f velocity;
    bool isOnGround;
    // ===================================

    std::unique_ptr<HealthBar> healthBar;
    bool showHealthBar;

    float speed;
    int health;
    const int maxHealth; // Max health for this enemy type, set by derived class

    EnemyState currentState;
    std::map<EnemyState, Animation> animations; // Loaded by derived class
    int currentFrame;
    float animationTimer; // Renamed from elapsedTime for clarity within animation context
    float direction; // 1.0f for right, -1.0f for left
    float baseScale;

    bool isAlive;
    bool isHurting;
    sf::Clock hurtTimer;
    float hurtDuration; // Duration of the hurt state visual/invulnerability

    // Helper to change state and reset animation
    void setState(EnemyState newState);
    void updateFacingDirection(const sf::Vector2f& playerPos);
    virtual void adjustSpriteOriginAndScale(); // For flipping sprite

};