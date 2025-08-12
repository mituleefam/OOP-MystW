#include "Striker.h"
#include "Player.h"
#include "Math.h" // For normalizeVector, assuming it's in Math.h
#include <iostream> // For debugging
//#include <filesystem> // For file operations if needed, though not directly used in this snippet (C++17 or later)
const float VERTICAL_TOLERANCE = 50.0f; // Khoảng cách Y tối đa để coi là "cùng tầng"
// Define the static member for Striker animation info
const std::map<EnemyState, AnimSheetInfo> Striker::strikerAnimSheetInfos = {
    { EnemyState::Idle,        { "Idle.png",   96, 84, 7, 0.15f, true  }},
    { EnemyState::Running,     { "Run.png",    96, 84, 8, 0.08f,  true  }}, // Faster run animation
    { EnemyState::Attacking,   { "Attack.png", 96, 84, 11, 0.1f, false }}, // Non-looping attack
    { EnemyState::Hurt,        { "Hurt.png",   96, 84, 4, 0.15f, false }},
    { EnemyState::Dying,       { "Die.png",    96, 84, 12, 0.5f,  false }}
};

Striker::Striker(const std::string& assetBaseFolder, float startX, float startY)
    : Enemy(startX, startY, 5, 120.0f), // Base constructor: x, y, max health, speed
    baseAssetPath(assetBaseFolder),
    attackInterval(1.f), // Striker attacks every second
    attackRange(160.0f)  // Striker attacks if player is within 170px
{
    baseScale = 3.0f;
	speed = 500.0f; // Override speed for Striker

    hurtDuration = 0.6f; // Strikers recover a bit faster from hurt animation
    loadSpecificAssets();
    setState(EnemyState::Idle); // Set initial state after loading assets
    attackCooldownTimer.restart();
    damageDealtInCurAttack = false;
}

void Striker::loadSpecificAssets() {
    // Load Striker-specific animations
    for (const auto& pair : strikerAnimSheetInfos) {
        loadStateFrames(pair.first, pair.second);
    }
    // === ĐỊNH NGHĨA HITBOX CHO STRIKER ===
    const float FRAME_WIDTH = 96.f;
    const float FRAME_HEIGHT = 84.f;
    float hitboxWidth = 33.f;
    float hitboxHeight = 75.f;
    localHitbox = sf::FloatRect(
        (FRAME_WIDTH - hitboxWidth) / 2.f, // left (tự động căn giữa)
        FRAME_HEIGHT - hitboxHeight,         // top (tự động đặt sát đáy)
        hitboxWidth,
        hitboxHeight
    );
    // ===================================
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

void Striker::update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) {
    Enemy::update(deltaTime, player, collisionLayer);
}

void Striker::updateAI(float deltaTime, Player& player) {

    sf::FloatRect playerHitBox = player.getHitBox();
    sf::FloatRect enemyHitBox = getHitBox();
    float playerBottom = playerHitBox.top + playerHitBox.height;
    float enemyBottom = enemyHitBox.top + enemyHitBox.height;

    if (std::abs(playerBottom - enemyBottom) > VERTICAL_TOLERANCE) {

        setState(EnemyState::Idle);
        velocity.x = 0;
        return;
    }

    // Prevent attacking if player is dead
    if (player.isDead()) { // Dòng MỚI
        setState(EnemyState::Idle);
        velocity.x = 0; // Đứng im khi player chết
        return;
    }

    // Basic AI: update facing direction, then decide to run or attack
    //updateFacingDirection(playerPos);
    sf::Vector2f playerPos = player.getPosition();
    updateFacingDirection(playerPos);

    if (currentState == EnemyState::Attacking) {
		velocity.x = 0; // Stop moving while attacking

        // === NEW DAMAGE LOGIC ===
        // Define which frame of the animation actually deals damage
        const int HIT_FRAME = 5; // Example: damage on the 5th frame

        // Check if the animation is on the hit frame AND we haven't dealt damage yet in this swing
        if (currentFrame == HIT_FRAME && !damageDealtInCurAttack) {
            // Now, check for collision
            if (getHitBox().intersects(player.getHitBox())) {
                player.takeDamage(5); // Deal damage
                damageDealtInCurAttack = true; // Mark damage as dealt
                std::cout << "Striker dealt damage on frame " << currentFrame << std::endl;
            }
        }
        return; // Don't do other AI logic while attacking
    }

    // If we are not in the attacking state, the flag from any previous attack must be reset.
    // attackRegistered = false;
    damageDealtInCurAttack = false;

    // Now decide what to do next: Attack or Run
    float distanceToPlayer = magnitude(playerPos - position); // True distance

    if (distanceToPlayer < attackRange && attackCooldownTimer.getElapsedTime().asSeconds() >= attackInterval) {
        std::cout << "Striker DECIDED to ATTACK" << std::endl;
        setState(EnemyState::Attacking);
        attackCooldownTimer.restart(); // Restart timer because we just initiated an attack
    }
    else {
        setState(EnemyState::Running);
        velocity.x = direction * speed;
    }
}

void Striker::performAttackLogic(Player& player) {
    // 1. Kiểm tra va chạm
    if (getHitBox().intersects(player.getHitBox())) {
        attackRegistered = true; // Đánh dấu là đòn đánh đã "chạm"
    }

    // 2. Chỉ gây sát thương MỘT LẦN DUY NHẤT
    if (attackRegistered && !damageDealtInCurAttack) {
        player.takeDamage(5);
        damageDealtInCurAttack = true; // Đánh dấu là đã gây sát thương trong lần vung kiếm này
    }
}

void Striker::draw(sf::RenderWindow& window) {
    // Call base class draw to draw the Striker's sprite
    Enemy::draw(window);
}

sf::FloatRect Striker::getHitBox() const {
    // Kích thước gốc của frame là 96x84
    // Định nghĩa hitbox trong không gian gốc (local space)
    float hitboxWidth = 35.f;  // Rộng 35 pixel
    float hitboxHeight = 40.f; // Cao 40 pixel
    float hitboxLeft = (96.f - hitboxWidth) / 2.f; // Căn giữa
    float hitboxTop = 84.f - hitboxHeight - 23.f;         // Đặt dưới cùng

    sf::FloatRect localHitbox(hitboxLeft, hitboxTop, hitboxWidth, hitboxHeight);

    // Dùng phép biến đổi của sprite để lấy hitbox toàn cục
    return sprite.getTransform().transformRect(localHitbox);
}