#include "Elf.h"
#include "Player.h"
#include "Math.h" // For normalizeVector, assuming it's in Math.h
#include <iostream> // For debugging

const float VERTICAL_TOLERANCE = 50.0f; // Khoảng cách Y tối đa để coi là "cùng tầng"
// Define the static member for Elf animation info
const std::map<EnemyState, AnimSheetInfo> Elf::elfAnimSheetInfos = {
    { EnemyState::Idle,        { "Idle.png",   160, 100, 6, 0.15f, true  }},
    { EnemyState::Running,     { "Run.png",    160, 100, 8, 0.1f,  true  }}, // Faster run animation
    { EnemyState::Attacking,   { "Attack.png", 160, 100, 6, 0.19f, false }}, // Non-looping attack
    { EnemyState::Hurt,        { "Hurt.png",   160, 100, 4, 0.15f, false }},
    { EnemyState::Dying,       { "Die.png",    160, 100, 7, 0.2f,  false }}
};

Elf::Elf(const std::string& assetBaseFolder, float startX, float startY)
    : Enemy(startX, startY, 5, 50.0f), // Base constructor: x, y, max health, speed
    baseAssetPath(assetBaseFolder),
    shootInterval(1.2f), // Elf shoots every 0.8 second
    attackRange(700.0f)  // Elf attacks if player is within 700px
{
    baseScale = 3.0f;
    hurtDuration = 0.6f; // Elves recover a bit faster from hurt animation
    loadSpecificAssets();
    setState(EnemyState::Idle); // Set initial state after loading assets
    shootCooldownTimer.restart();
}

void Elf::loadSpecificAssets() {
    // Load Elf-specific animations
    for (const auto& pair : elfAnimSheetInfos) {
        loadStateFrames(pair.first, pair.second);
    }

    // Load arrow texture
    std::string arrowPath = baseAssetPath + "/Arrow.png";
    arrowTexture = std::make_shared<sf::Texture>();
    if (!arrowTexture->loadFromFile(arrowPath)) {
        std::cerr << "Failed to load Elf arrow texture: " << arrowPath << std::endl;
    }
    // Set initial sprite texture and rect from Idle state
    if (animations.count(EnemyState::Idle) && !animations[EnemyState::Idle].frames.empty()) {
        sprite.setTexture(*animations[EnemyState::Idle].sheet);
        sprite.setTextureRect(animations[EnemyState::Idle].frames[0]);
        adjustSpriteOriginAndScale(); // Call this after texture is set
    }
    else {
        std::cerr << "Elf Idle animation not loaded, sprite texture not set." << std::endl;
    }
}

void Elf::loadStateFrames(EnemyState state, const AnimSheetInfo& info) {
    Animation anim;
    anim.frameDuration = info.frameDuration;
    anim.loop = info.loop;

    std::string sheetPath = baseAssetPath + "/" + info.filename;
    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile(sheetPath)) {
        std::cerr << "Failed to load Elf sheet: " << sheetPath << std::endl;
        return;
    }
    anim.sheet = texture;

    for (int i = 0; i < info.frameCount; ++i) {
        anim.frames.push_back(sf::IntRect(i * info.frameWidth, 0, info.frameWidth, info.frameHeight));
    }
    animations[state] = anim;
}

void Elf::update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) {
    // Always update arrows, even if Elf is hurt or dying
    for (auto& arrow : arrows) {
        arrow.update(deltaTime);
    }
	// Check for arrow collisions with the player
    for (auto& arrow : arrows) {
        // Nếu mũi tên chưa hết hạn VÀ va chạm với hitbox của player
        if (!arrow.isExpired() && arrow.getHitBox().intersects(player.getHitBox())) {
            player.takeDamage(10); // Player nhận 10 sát thương
            arrow.lifetime = 0;   // Đặt lifetime = 0 để đánh dấu xóa mũi tên này ngay
            std::cout << "Player hit by Elf arrow!" << std::endl;
            break; // Thoát khỏi vòng lặp, mỗi frame chỉ xử lý 1 va chạm
        }
    }
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(),
        [](const AnimatedProjectile& a) {
            return a.isExpired();
        }), arrows.end());

    // Now call the base Enemy update for normal behavior
    Enemy::update(deltaTime, player, collisionLayer);
}

void Elf::updateAI(float deltaTime, Player& player) {

    sf::FloatRect playerHitBox = player.getHitBox();
    sf::FloatRect enemyHitBox = getHitBox();
    float playerBottom = playerHitBox.top + playerHitBox.height;
    float enemyBottom = enemyHitBox.top + enemyHitBox.height;
    if (std::abs(playerBottom - enemyBottom) > VERTICAL_TOLERANCE) {
        // Player ở tầng khác, đứng im
        setState(EnemyState::Idle);
        velocity.x = 0; // Dừng di chuyển ngang
        return;         // Không xử lý AI gì thêm
    }
    // =============================
    
    // Prevent attacking if player is dead
    if (player.isDead()) { // Dòng MỚI
        setState(EnemyState::Idle);
        velocity.x = 0; // Đứng im khi player chết
        return;
    }

    // Basic AI: update facing direction, then decide to run or attack
	sf::Vector2f playerPos = player.getPosition();
    updateFacingDirection(playerPos);

    float distanceToPlayer = magnitude(playerPos - position); // True distance

    if (currentState == EnemyState::Attacking) {
		velocity.x = 0; // Stop moving while attacking
        // The base animate() will transition to Idle when attack anim finishes if it's non-looping.
        // performAttackLogic is responsible for the actual shot during this state if cooldown allows.
        //const int SHOOT_FRAME = 2; // Ví dụ: Bắn tên ở frame thứ 2

        //if (currentFrame == SHOOT_FRAME && !shotFiredInCurrentAttack) {
            performAttackLogic(player);
        //    shotFiredInCurrentAttack = true;
        //}
        return; // Don't change state or move if in attack animation
    }

    if (distanceToPlayer < attackRange) {
        setState(EnemyState::Attacking);
    }
    else {
        setState(EnemyState::Running);
        velocity.x = direction * speed;
    }
}


void Elf::performAttackLogic(Player& player) {
    // This is called when the Elf decides to attack (e.g., when state becomes Attacking)
    if (shootCooldownTimer.getElapsedTime().asSeconds() >= shootInterval) {
        tryShootArrow(player.getPosition(), player.getHitBox());
        shootCooldownTimer.restart();
    }
}


void Elf::tryShootArrow(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) {
    if (!arrowTexture || arrowTexture->getSize().x == 0) { // Check if texture is valid
        std::cerr << "Elf: Arrow texture not loaded or invalid." << std::endl;
        return;
    }

    sf::Vector2f arrowStartPos = position; // Adjust Y to match arrow sprite origin if needed
    // Example: arrowStartPos.y -= sprite.getGlobalBounds().height * 0.3f; // Adjust based on elf sprite
    arrowStartPos.y -= 80.f; // Manual adjustment based on your old code's visual

    sf::Vector2f shootDir(direction, 0.f);

    normalizeVector(shootDir); // From your Math.h

    float arrowSpeed = 600.0f;
    sf::Vector2f arrowVelocity = shootDir * arrowSpeed;

    // AnimatedProjectile expects a vector of textures, even if it's just one for a non-animated arrow
    std::vector<std::shared_ptr<sf::Texture>> arrowFramesVec = { arrowTexture };
    arrows.emplace_back(arrowFramesVec, arrowStartPos, arrowVelocity, 0.1f); // 0.1f is frame duration if arrow was animated
    std::cout << "Elf fired an arrow!" << std::endl;
}

void Elf::updateArrows(float deltaTime, const sf::RenderWindow& window) {
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

void Elf::draw(sf::RenderWindow& window) {
    // Call base class draw to draw the Elf's sprite
    Enemy::draw(window);

    // Draw Elf's arrows
    for (const auto& arrow : arrows) {
        arrow.draw(window);
    }
}

sf::FloatRect Elf::getHitBox() const {
    // Kích thước gốc của frame là 160x100
    float hitboxWidth = 40.f;
    float hitboxHeight = 45.f;
    float hitboxLeft = (160.f - hitboxWidth) / 2.f;
	float hitboxTop = 100.f - hitboxHeight - 5; // 5 pixels above the bottom

    sf::FloatRect localHitbox(hitboxLeft, hitboxTop, hitboxWidth, hitboxHeight);
    return sprite.getTransform().transformRect(localHitbox);
}

bool Elf::checkArrowCollisions(const sf::FloatRect& playerBounds) {
    for (const auto& arrow : arrows) {
        if (arrow.getHitBox().intersects(playerBounds)) {
            return true;
        }
    }
    return false;
}

void Elf::removeArrowsCollidingWith(const sf::FloatRect& playerBounds) {
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(),
        [&](const AnimatedProjectile& arrow) {
            return arrow.getHitBox().intersects(playerBounds);
        }), arrows.end());
}