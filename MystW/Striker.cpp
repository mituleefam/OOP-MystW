#include "Striker.h"
#include "Player.h"
#include "Math.h" // For normalizeVector, assuming it's in Math.h
#include <iostream> // For debugging
//#include <filesystem> // For file operations if needed, though not directly used in this snippet (C++17 or later)

// Define the static member for Striker animation info
const std::map<EnemyState, AnimSheetInfo> Striker::strikerAnimSheetInfos = {
    { EnemyState::Idle,        { "Idle.png",   96, 84, 7, 0.15f, true  }},
    { EnemyState::Running,     { "Run.png",    96, 84, 8, 0.1f,  true  }}, // Faster run animation
    { EnemyState::Attacking,   { "Attack.png", 96, 84, 6, 0.18f, false }}, // Non-looping attack
    { EnemyState::Hurt,        { "Hurt.png",   96, 84, 4, 0.15f, false }},
    { EnemyState::Dying,       { "Die.png",    96, 84, 12, 0.5f,  false }}
};

Striker::Striker(const std::string& assetBaseFolder, float startX, float startY)
    : Enemy(startX, startY, 20, 120.0f), // Base constructor: x, y, max health, speed
    baseAssetPath(assetBaseFolder),
    attackInterval(1.0f), // Striker attacks every 0.8 second
    attackRange(250.0f)  // Striker attacks if player is within 700px
{
    hurtDuration = 0.6f; // Elves recover a bit faster from hurt animation
    loadSpecificAssets();
    setState(EnemyState::Idle); // Set initial state after loading assets
    attackCooldownTimer.restart();
}

void Striker::loadSpecificAssets() {
    // Load Striker-specific animations
    for (const auto& pair : strikerAnimSheetInfos) {
        loadStateFrames(pair.first, pair.second);
    }

    // Set initial sprite texture and rect from Idle state
    if (animations.count(EnemyState::Idle) && !animations[EnemyState::Idle].frames.empty()) {
        sprite.setTexture(*animations[EnemyState::Idle].sheet);
        sprite.setTextureRect(animations[EnemyState::Idle].frames[0]);
        adjustSpriteOriginAndScale(); // Call this after texture is set
    }
    else {
        std::cerr << "Striker Idle animation not loaded, sprite texture not set." << std::endl;
    }
}

void Striker::loadStateFrames(EnemyState state, const AnimSheetInfo& info) {
    Animation anim;
    anim.frameDuration = info.frameDuration;
    anim.loop = info.loop;

    std::string sheetPath = baseAssetPath + "/" + info.filename;
    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile(sheetPath)) {
        std::cerr << "Failed to load Striker sheet: " << sheetPath << std::endl;
        return;
    }
    anim.sheet = texture;

    for (int i = 0; i < info.frameCount; ++i) {
        anim.frames.push_back(sf::IntRect(i * info.frameWidth, 0, info.frameWidth, info.frameHeight));
    }
    animations[state] = anim;
}

void Striker::update(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    // Now call the base Enemy update for normal behavior
    Enemy::update(deltaTime, playerPos, playerHitBox);
}

void Striker::updateAI(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    // Prevent attacking if player is dead (hitbox is zero)
    if (playerHitBox.width == 0.f && playerHitBox.height == 0.f) {
        setState(EnemyState::Idle);
        return;
    }

    // Basic AI: update facing direction, then decide to run or attack
    updateFacingDirection(playerPos);

    if (currentState == EnemyState::Attacking) {
        // The base animate() will transition to Idle when attack anim finishes if it's non-looping.
        // performAttackLogic is responsible for the actual shot during this state if cooldown allows.
        performAttackLogic(playerPos, playerHitBox); // Check if it can attack again
        return; // Don't change state or move if in attack animation
    }

    // If we are not in the attacking state, the flag from any previous attack must be reset.
    attackRegistered = false;

    // Now decide what to do next: Attack or Run
    float distanceToPlayer = magnitude(playerPos - position); // True distance

    if (distanceToPlayer < attackRange && attackCooldownTimer.getElapsedTime().asSeconds() >= attackInterval) {
        setState(EnemyState::Attacking);
        attackCooldownTimer.restart(); // Restart timer because we just initiated an attack
    }
    else {
        setState(EnemyState::Running);
        position.x += direction * speed * deltaTime; // Move towards/away logic can be more complex
        // For now, simple horizontal move in current direction
    }
}


//void Striker::performAttackLogic(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
//    // This is called when the Striker decides to attack (e.g., when state becomes Attacking)
//    if (attackCooldownTimer.getElapsedTime().asSeconds() >= attackInterval && !attackRegistered) {
//        if (getHitBox().intersects(playerHitBox)) {
//            attackRegistered = true;
//            std::cout << "Striker attacks!\n";
//        }
//        attackCooldownTimer.restart();
//    }
//    // Reset flag when attack is done
//    if (currentState != EnemyState::Attacking) {
//        attackRegistered = false;
//    }
//}

void Striker::performAttackLogic(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    // This is called every frame of the attack animation.
    // We only register the hit ONCE per animation.
    if (!attackRegistered) { // If we haven't landed a hit yet during this attack
        if (getHitBox().intersects(playerHitBox)) {
            attackRegistered = true;
            // DEBUG MESSAGE TO CONFIRM HIT
            std::cout << "Striker attack connected with player hitbox!\n";
        }
    }
    // The flag will be reset in updateAI once the state changes away from Attacking.
}

void Striker::draw(sf::RenderWindow& window) {
    // Call base class draw to draw the Striker's sprite
    Enemy::draw(window);
}

//bool Striker::checkAttackCollisions(const sf::FloatRect& playerHitBox) const {
//    // Check if the striker's hitbox intersects with the player's hitbox
//    sf::FloatRect strikerHitBox = getHitBox();
//    return strikerHitBox.intersects(playerHitBox);
//}