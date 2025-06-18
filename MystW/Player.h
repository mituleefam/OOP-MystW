#pragma once
#include "Enemy.h"
#include <SFML/Graphics.hpp>

class Player
{
public:
    enum class AnimationState { Idle, Running, Jumping, AirAttacking, Attacking, Hurting, Die };

private:
    sf::Texture texture;
    sf::Texture textureSheet; // For sprite sheet animations

    sf::Vector2f velocity; // Horizontal and vertical velocity
    bool isJumping = false;
    const float jumpSpeed = 10.0f;
    const float moveSpeed = 7.0f; // Speed of horizontal movement
    const float gravity = 0.15f;
    const float groundY = 700.0f; // Same as the initial position of the player sprite

    bool isAttacking = false;
    float attackCooldown = 0.0f; // Cooldown for attacks
    const float attackCooldownDuration = 1.1f; // 1.1 second cooldown for attacks
    bool isFacingRight = true; // Direction the player is facing

    // Animation variables
    AnimationState animState = AnimationState::Idle;
    int currentFrame = 0;
    float animationTimer = 0.0f;
    float frameDuration = 0.2f; // seconds per frame
    sf::IntRect idleFrames[4]; // array of frame rects
    sf::IntRect runningFrames[6];
    sf::IntRect jumpingFrames[4];
    //sf::IntRect handAttackingFrames[3];  
	sf::IntRect airAttackingFrames[7]; // Frames for air attack
    sf::IntRect swordAttackingFrames[11];
    sf::IntRect hurtingFrames[3];
    sf::IntRect dieFrames[6];

    const float knockbackForce = 1.0f; // Force applied when the player is attacked

public:
    int health = 100; // Player health

    sf::Sprite sprite;
    //sf::Sprite spriteSheet; // For sprite sheet animations
    sf::Vector2f getPosition() const { return sprite.getPosition(); } // Get player position
    void setPosition(float x, float y) { sprite.setPosition(x, y); } // Set player position
    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
    sf::FloatRect getHitBox() const; // Get the hitbox of the player for collision detection

    Player() : velocity(0.0f, 0.0f) {};
    void Initialize(); // called once per game
    void Load(); // called once per level
    void Update(float deltaTime); // called every frame
    //void Attack(); // called when the player attacks
    void Draw(); // called every frame after Update

    bool isAttacked = false;
    bool attackRegistered = false; // To prevent multiple attack registrations
    bool playerIsAttacking() const { return isAttacking; }

    sf::FloatRect getAttackBounds() const {
        //     if (animState != AnimationState::Attacking)
                 //return sf::FloatRect(); // No attack bounds if not attacking;
                 // No need cause in main loop we check if the player is attacking before calling this function
        float width = 10.f;
        float height = 20.f;
        sf::Vector2f pos = sprite.getPosition();
        float offsetX = isFacingRight ? 15.f : -45.f; // 
        return sf::FloatRect(pos.x + offsetX, pos.y - height, width, height);
    }

    void takeDamage(int damage);
    float hurtCooldown = 0.0f;
    const float hurtCooldownDuration = 1.0f; // 1 second of invincibility
};

