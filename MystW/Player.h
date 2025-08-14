#pragma once
#include "Enemy.h"
#include <SFML/Graphics.hpp>
#include "CollisionLayer.hpp" // ADDED: Forward-declare or include

class Player
{
public:
    enum class AnimationState { Idle, Running, Jumping, AirAttacking, Attacking, Hurting, Die };

private:
    sf::Vector2f velocity; // Horizontal and vertical velocity
    
    const float jumpSpeed = 900.0f;
    const float moveSpeed = 600.0f; // Speed of horizontal movement
    const float gravity = 1600.0f;
	const float fastFallGravity = 3000.0f; // Gravity when fast falling

    float attackCooldown = 0.0f; // Cooldown for attacks
    const float attackCooldownDuration = 1.1f; // 1 second cooldown for attacks
    float hurtCooldown = 0.0f;
    const float hurtCooldownDuration = 1.0f; // 1 second of invincibility

    bool isFacingRight = true; // Direction the player is facing

    // Animation variables
    sf::Texture texture;
    sf::Texture textureSheet; // For sprite sheet animations
    AnimationState animState = AnimationState::Idle;
    int currentFrame = 0;
    float animationTimer = 0.0f;
    float frameDuration = 0.2f; // seconds per frame
	// Animation frame rectangles
    sf::IntRect idleFrames[4];
    sf::IntRect runningFrames[6];
    sf::IntRect jumpingFrames[4]; 
	sf::IntRect airAttackingFrames[7];
    sf::IntRect swordAttackingFrames[11];
    sf::IntRect hurtingFrames[3];
    sf::IntRect dieFrames[6];
    float baseScale;
	// Unused variables for future use
    const float knockbackForce = 1.0f; // Force applied when the player is attacked
    sf::FloatRect localHitbox;
public:
    int health = 100; // Player health

    sf::Sprite sprite;
    sf::Vector2f getPosition() const { return sprite.getPosition(); } // Get player position
    void setPosition(float x, float y) { sprite.setPosition(x, y); } // Set player position
    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
    sf::FloatRect getHitBox() const; // Get the hitbox of the player for collision detection

    Player() : velocity(0.0f, 0.0f), baseScale(3.5f) {};
    void Initialize(); // called once per game
    void Load(); // called once per level
    void Update(float deltaTime, CollisionLayer& collisionLayer); // called every frame
    void Draw(sf::RenderWindow& window);

    bool isJumping = false;
    bool isAttacking = false;
    bool isAttacked = false;
    bool attackRegistered = false; // To prevent multiple attack registrations
    sf::FloatRect getAttackBounds() const;
    void takeDamage(int damage);
    bool isDead() const;
    AnimationState getAnimState() const { return animState; }
};

