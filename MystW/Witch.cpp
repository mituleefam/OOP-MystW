#include "Witch.h"
#include "Player.h"
#include "Math.h" // For normalizeVector, assuming it's in Math.h
#include <iostream> // For debugging
//#include <filesystem> // For file operations if needed, though not directly used in this snippet (C++17 or later)

// Define the static member for Witch animation info
const std::map<EnemyState, AnimSheetInfo> Witch::witchAnimSheetInfos = {
    { EnemyState::Idle,        { "Idle.png",   160, 100, 6, 0.15f, true  }},
    { EnemyState::Running,     { "Run.png",    160, 100, 8, 0.1f,  true  }}, // Faster run animation
    { EnemyState::Attacking,   { "Attack.png", 160, 100, 6, 0.15f, false }}, // Non-looping attack
    { EnemyState::Hurt,        { "Hurt.png",   160, 100, 4, 0.15f, false }},
    { EnemyState::Dying,       { "Die.png",    160, 100, 7, 0.2f,  false }}
};

Witch::Witch(const std::string& assetBaseFolder, float startX, float startY)
    : Enemy(startX, startY, 20, 120.0f), // Base constructor: x, y, max health, speed
    baseAssetPath(assetBaseFolder),
    shootInterval(1.0f), // Witch shoots every 0.8 second
    attackRange(700.0f)  // Witch attacks if player is within 700px
{
    hurtDuration = 0.6f; // Elves recover a bit faster from hurt animation
    loadSpecificAssets();
    setState(EnemyState::Idle); // Set initial state after loading assets
    shootCooldownTimer.restart();
}

void Witch::loadSpecificAssets() {
    // Load Witch-specific animations
    for (const auto& pair : witchAnimSheetInfos) {
        loadStateFrames(pair.first, pair.second);
    }

    // Load arrow texture
    std::string arrowPath = baseAssetPath + "/Arrow.png";
    arrowTexture = std::make_shared<sf::Texture>();
    if (!arrowTexture->loadFromFile(arrowPath)) {
        std::cerr << "Failed to load Witch arrow texture: " << arrowPath << std::endl;
    }
    // Set initial sprite texture and rect from Idle state
    if (animations.count(EnemyState::Idle) && !animations[EnemyState::Idle].frames.empty()) {
        sprite.setTexture(*animations[EnemyState::Idle].sheet);
        sprite.setTextureRect(animations[EnemyState::Idle].frames[0]);
        adjustSpriteOriginAndScale(); // Call this after texture is set
    }
    else {
        std::cerr << "Witch Idle animation not loaded, sprite texture not set." << std::endl;
    }
}

void Witch::loadStateFrames(EnemyState state, const AnimSheetInfo& info) {
    Animation anim;
    anim.frameDuration = info.frameDuration;
    anim.loop = info.loop;

    std::string sheetPath = baseAssetPath + "/" + info.filename;
    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile(sheetPath)) {
        std::cerr << "Failed to load Witch sheet: " << sheetPath << std::endl;
        return;
    }
    anim.sheet = texture;

    for (int i = 0; i < info.frameCount; ++i) {
        anim.frames.push_back(sf::IntRect(i * info.frameWidth, 0, info.frameWidth, info.frameHeight));
    }
    animations[state] = anim;
}

void Witch::update(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    // Always update arrows, even if Witch is hurt or dying
    for (auto& arrow : arrows) {
        arrow.update(deltaTime);
    }
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(),
        [](const AnimatedProjectile& a) {
            sf::FloatRect b = a.getBounds();
            return b.left > 1920 || (b.left + b.width) < 0 ||
                b.top > 1080 || (b.top + b.height) < 0;
        }), arrows.end());

    // Now call the base Enemy update for normal behavior
    Enemy::update(deltaTime, playerPos, playerHitBox);
}

void Witch::updateAI(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    // Prevent attacking if player is dead (hitbox is zero)
    if (playerHitBox.width == 0.f && playerHitBox.height == 0.f) {
        setState(EnemyState::Idle);
        return;
    }

    // Update arrows first (movement and out-of-bounds removal)
    // For out-of-bounds, ideally use game world dimensions, not direct window ref here.
    // We'll simulate fixed bounds for arrow removal.
    for (auto& arrow : arrows) {
        arrow.update(deltaTime);
    }
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(),
        [](const AnimatedProjectile& a) {
            sf::FloatRect b = a.getBounds();
            // Assuming a fixed game world size for arrow removal
            return b.left > 1920 || (b.left + b.width) < 0 ||
                b.top > 1080 || (b.top + b.height) < 0;
        }), arrows.end());


    // Basic AI: update facing direction, then decide to run or attack
    updateFacingDirection(playerPos);

    float distanceToPlayer = magnitude(playerPos - position); // True distance

    if (currentState == EnemyState::Attacking) {
        // The base animate() will transition to Idle when attack anim finishes if it's non-looping.
        // performAttackLogic is responsible for the actual shot during this state if cooldown allows.
        performAttackLogic(playerPos, playerHitBox); // Check if it can shoot again
        return; // Don't change state or move if in attack animation
    }

    if (distanceToPlayer < attackRange) {
        setState(EnemyState::Attacking);
        // performAttackLogic(playerPos); // Initial shot attempt when entering attack state
    }
    else {
        setState(EnemyState::Running);
        position.x += direction * speed * deltaTime; // Move towards/away logic can be more complex
        // For now, simple horizontal move in current direction
    }
}


void Witch::performAttackLogic(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    // This is called when the Witch decides to attack (e.g., when state becomes Attacking)
    if (shootCooldownTimer.getElapsedTime().asSeconds() >= shootInterval) {
        tryShootArrow(playerPos, playerHitBox);
        shootCooldownTimer.restart();
    }
}


void Witch::tryShootArrow(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    if (!arrowTexture || arrowTexture->getSize().x == 0) { // Check if texture is valid
        std::cerr << "Witch: Arrow texture not loaded or invalid." << std::endl;
        return;
    }

    sf::Vector2f arrowStartPos = position; // Adjust Y to match arrow sprite origin if needed
    // Example: arrowStartPos.y -= sprite.getGlobalBounds().height * 0.3f; // Adjust based on elf sprite
    arrowStartPos.y -= 80.f; // Manual adjustment based on your old code's visual

    sf::Vector2f shootDir(direction, 0.f); // = playerPos - arrowStartPos;
    // Minor vertical adjustment to aim towards player's center, not feet
    // shootDir.y -= playerHitBox.height / 2.f; // Requires player reference or its hitbox passed
    // For simplicity, let's use the playerPos as is for now or a fixed offset
    // shootDir.y -= 30.f; // A fixed offset aiming slightly up

    normalizeVector(shootDir); // From your Math.h

    float arrowSpeed = 650.0f;
    sf::Vector2f arrowVelocity = shootDir * arrowSpeed;

    // AnimatedProjectile expects a vector of textures, even if it's just one for a non-animated arrow
    std::vector<std::shared_ptr<sf::Texture>> arrowFramesVec = { arrowTexture };
    arrows.emplace_back(arrowFramesVec, arrowStartPos, arrowVelocity, 0.1f); // 0.1f is frame duration if arrow was animated
    std::cout << "Witch fired an arrow!" << std::endl;
}

void Witch::updateArrows(float deltaTime, const sf::RenderWindow& window) {
    for (auto& arrow : arrows) {
        arrow.update(deltaTime);
    }
    // Remove arrows that are out of bounds
    sf::Vector2u windowSize = window.getSize();
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(),
        [&](const AnimatedProjectile& a) {
            sf::FloatRect b = a.getBounds();
            return b.left > windowSize.x || (b.left + b.width) < 0 ||
                b.top > windowSize.y || (b.top + b.height) < 0;
        }), arrows.end());
}

void Witch::draw(sf::RenderWindow& window) {
    // Call base class draw to draw the Witch's sprite
    Enemy::draw(window);

    // Draw Witch's arrows
    for (const auto& arrow : arrows) {
        arrow.draw(window);
    }
}

bool Witch::checkArrowCollisions(const sf::FloatRect& playerBounds) {
    for (const auto& arrow : arrows) {
        if (arrow.getHitBox().intersects(playerBounds)) {
            return true;
        }
    }
    return false;
}

void Witch::removeArrowsCollidingWith(const sf::FloatRect& playerBounds) {
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(),
        [&](const AnimatedProjectile& arrow) {
            return arrow.getHitBox().intersects(playerBounds);
        }), arrows.end());
}