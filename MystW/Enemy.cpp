#include "Enemy.h"
#include <iostream> // For debugging

// Constructor
Enemy::Enemy(float startX, float startY, int initialHealth, float enemySpeed)
    : position(startX, startY),
    health(initialHealth),
    maxHealth(initialHealth),
    speed(enemySpeed),
    currentState(EnemyState::Idle),
    currentFrame(0),
    animationTimer(0.f),
    direction(1.0f),
    isAlive(true),
    isHurting(false),
    hurtDuration(0.7f) // Default hurt duration, can be overridden
{
    sprite.setPosition(position);
    // Derived class will call loadSpecificAssets() and then likely setState(EnemyState::Idle)
}

void Enemy::update(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    if (!isAlive) {
        return;
    }

    // Dying state handling
    if (currentState == EnemyState::Dying) {
        animate(deltaTime);
        const Animation& anim = animations[currentState];
        // Check if dying animation has finished
        if (!anim.frames.empty() && !anim.loop && currentFrame >= static_cast<int>(anim.frames.size()) - 1) {
            isAlive = false; // Now truly dead and won't be updated/drawn further (handled in main loop)
        }
        return; // No other logic if dying
    }

    // Hurting state handling
    if (isHurting) {
        animate(deltaTime);
        if (hurtTimer.getElapsedTime().asSeconds() > hurtDuration) {
            isHurting = false;
            if (isAlive) { // Only go to idle if still alive
                setState(EnemyState::Idle);
            }
        }
        return; // No other logic if hurting
    }

    // Let derived class handle specific AI (movement, attacking decisions)
    updateAI(deltaTime, playerPos, playerHitBox);

    // Update sprite position based on internal position
    sprite.setPosition(position);
    // Animate current state (Idle, Running, Attacking)
    animate(deltaTime);
}

void Enemy::draw(sf::RenderWindow& window) {
    if (!isAlive && currentState != EnemyState::Dying) { // Don't draw if dead, unless it's the dying animation itself
        return;
    }
    if (currentState == EnemyState::Dying && !animations[EnemyState::Dying].frames.empty() && currentFrame >= static_cast<int>(animations[EnemyState::Dying].frames.size()) - 1) {
        // If dying animation is finished, and isAlive is already set to false, don't draw.
        if (!isAlive) return;
    }
    window.draw(sprite);
}

void Enemy::takeDamage(int damageAmount) {
    if (!isAlive || isHurting || currentState == EnemyState::Dying) {
        return; // Can't take damage if already dead, hurting, or in the process of dying
    }

    health -= damageAmount;
    isHurting = true;
    hurtTimer.restart();
    std::cout << "Enemy took " << damageAmount << " damage. Health: " << health << std::endl;


    if (health <= 0) {
        health = 0;
        setState(EnemyState::Dying);
        isHurting = false; // Stop hurting state as dying takes precedence
        std::cout << "Enemy is Dying." << std::endl;
    }
    else {
        setState(EnemyState::Hurt);
    }
}

void Enemy::animate(float deltaTime) {
    if (animations.find(currentState) == animations.end() || animations[currentState].frames.empty()) {
        return; // No animation defined for current state or no frames
    }

    const Animation& currentAnim = animations[currentState];
    animationTimer += deltaTime;

    if (animationTimer >= currentAnim.frameDuration) {
        animationTimer -= currentAnim.frameDuration; // Subtract to keep overflow for next frame check if needed
        currentFrame++;

        if (currentFrame >= static_cast<int>(currentAnim.frames.size())) {
            if (currentAnim.loop) {
                currentFrame = 0;
            }
            else {
                currentFrame = static_cast<int>(currentAnim.frames.size()) - 1; // Stay on last frame
                if (currentState == EnemyState::Attacking) { // Example: transition from attacking to idle
                    setState(EnemyState::Idle);
                }
            }
        }
        sprite.setTextureRect(currentAnim.frames[currentFrame]);
    }
}

void Enemy::setState(EnemyState newState) {
    if (currentState != newState || (newState == EnemyState::Hurt && !isHurting) || (newState == EnemyState::Attacking && currentState != EnemyState::Attacking)) {
        // Allow re-triggering attack/hurt if not already in it
        currentState = newState;
        currentFrame = 0;
        animationTimer = 0.f;

        if (animations.count(currentState) && !animations[currentState].frames.empty()) {
            sprite.setTexture(*animations[currentState].sheet); // Ensure correct sheet is set
            sprite.setTextureRect(animations[currentState].frames[0]);
            adjustSpriteOriginAndScale(); // Adjust origin for new frame size potentially
        }
        else {
            // std::cerr << "Warning: No animation frames for state " << static_cast<int>(newState) << std::endl;
        }
    }
}

void Enemy::updateFacingDirection(const sf::Vector2f& playerPos) {
    if (playerPos.x < position.x) {
        direction = -1.0f; // Player is to the left
    }
    else {
        direction = 1.0f; // Player is to the right
    }
    adjustSpriteOriginAndScale();
}

void Enemy::adjustSpriteOriginAndScale() {
    // This might need to be more sophisticated if frame sizes vary wildly
    // For now, assuming a consistent desired visual scale.
    // Derived classes might override this if their sprites need special handling.
    sf::FloatRect localBounds = sprite.getLocalBounds(); // Use local bounds for origin
    sprite.setOrigin(localBounds.width / 2.0f, localBounds.height); // Example: origin at bottom-center
    sprite.setScale(direction * 4.0f, 4.0f); // Use the general scale from your old code
}


bool Enemy::isDead() const {
    return !isAlive && currentState == EnemyState::Dying && currentFrame >= static_cast<int>(animations.at(EnemyState::Dying).frames.size()) - 1;
    // A more robust check for "completely finished dying and can be removed"
    // The main loop can also just check `!isAlive` after `update` and if the dying animation is visually over.
    // For removal from vector, `!isAlive` after its final animation frame is key.
}

sf::Vector2f Enemy::getPosition() const {
    return position;
}

sf::FloatRect Enemy::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::FloatRect Enemy::getHitBox() const {
    // Generic hitbox calculation, can be overridden by derived classes if needed
    sf::FloatRect box = sprite.getGlobalBounds();
    float shrinkHorizontal = 0.5f; // Shrink more horizontally for a taller sprite
    float shrinkVertical = 0.2f;   // Shrink less vertically

    box.left += box.width * (1.0f - shrinkHorizontal) / 2.0f;
    box.width *= shrinkHorizontal;
    //box.top += box.height * (1.0f - shrinkVertical) / 2.0f;
    box.top += box.height * 0.7f; // lower multiplicant if aim higher
    box.height *= shrinkVertical;
    return box;
}

bool Enemy::checkIsHurting() const {
    return isHurting;
}