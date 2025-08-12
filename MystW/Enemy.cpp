#include "Enemy.h"
#include "Player.h"
#include <iostream> // For debugging
const float GRAVITY = 2500.f;
// Constructor
Enemy::Enemy(float startX, float startY, int initialHealth, float enemySpeed)
    : position(startX, startY),
    velocity(0.f, 0.f), // Khởi tạo velocity
    isOnGround(false),  // Ban đầu coi như đang ở trên không
    health(initialHealth),
    maxHealth(initialHealth),
    speed(enemySpeed),
    currentState(EnemyState::Idle),
    currentFrame(0),
    animationTimer(0.f),
    direction(1.0f),
    baseScale(3.0f),
    isAlive(true),
    isHurting(false),
    showHealthBar(false),
    hurtDuration(0.7f) // Default hurt duration, can be overridden
{
    sprite.setPosition(position);
    // Derived class will call loadSpecificAssets() and then likely setState(EnemyState::Idle)
    sf::Vector2f healthBarSize(50.f, 6.f);
    // Khởi tạo nhưng không truyền font để nó không vẽ text
    healthBar = std::make_unique<HealthBar>(maxHealth, sf::Vector2f(0, 0), healthBarSize, nullptr);
}

void Enemy::update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) {
    if (!isAlive) {
        return;
    }

    // Dying state handling
    if (currentState == EnemyState::Dying) {
        animate(deltaTime);
        return; // No other logic if dying
    }

    if (healthBar) {
        // Lấy biên của sprite để định vị thanh máu
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        // Căn giữa thanh máu và đặt nó phía trên đầu sprite 10 pixel
        sf::Vector2f healthBarPos(
            spriteBounds.left + (spriteBounds.width / 2.f) - (healthBar->getSize().x / 2.f), 
            spriteBounds.top - 15.f
        );
        healthBar->setPosition(healthBarPos);

        // Chỉ hiện thanh máu khi quái bị thương và chưa chết
        if (isHurting && health > 0) {
            showHealthBar = true;
        }
    }

    // Hurting state handling
    if (isHurting) {
        animate(deltaTime);
        if (hurtTimer.getElapsedTime().asSeconds() > hurtDuration) {
            isHurting = false;
            if (health > 0) { //isAlive) { // Only go to idle if still alive
                setState(EnemyState::Idle);
            }
        }
        return; // No other logic if hurting
    }

    // Let derived class handle specific AI (movement, attacking decisions)
    updateAI(deltaTime, player);//playerPos, playerHitBox);
    // Xử lý vật lý và va chạm
    handlePhysicsAndCollision(deltaTime, collisionLayer);

    // Update sprite position based on internal position
    sprite.setPosition(position);
    // Animate current state (Idle, Running, Attacking)
    animate(deltaTime);
}

void Enemy::draw(sf::RenderWindow& window) {
    if (isAlive || currentState == EnemyState::Dying) {
        window.draw(sprite);
        if (showHealthBar && healthBar) {
            healthBar->draw(window);
        }
    }
}

void Enemy::takeDamage(int damageAmount) {
    if (!isAlive || isHurting || currentState == EnemyState::Dying) {
        return; // Can't take damage if already dead, hurting, or in the process of dying

    }

    health -= damageAmount;
    isHurting = true;
    hurtTimer.restart();
    if (healthBar) {
        healthBar->setCurrentHealth(health);
    }
    std::cout << "Enemy took " << damageAmount << " damage. Health: " << health << std::endl;


    if (health <= 0) {
        health = 0;
        setState(EnemyState::Dying);
        isHurting = false; // Stop hurting state as dying takes precedence
        velocity = sf::Vector2f(0, 0); // Ngừng di chuyển khi chết
        std::cout << "Enemy is Dying." << std::endl;
    }
    else {
        setState(EnemyState::Hurt);
    }
}

void Enemy::animate(float deltaTime) {
    if (animations.find(currentState) == animations.end()) { //|| animations[currentState].frames.empty()) {
        return; // No animation defined for current state or no frames
    }

    const Animation& currentAnim = animations[currentState];
    // Kiểm tra xem animation có frame nào không
    bool hasFrames = (currentAnim.type == Animation::AnimationType::SpriteSheet && !currentAnim.frames.empty()) ||
        (currentAnim.type == Animation::AnimationType::TextureList && !currentAnim.textureFrames.empty());
    if (!hasFrames) return;
    animationTimer += deltaTime;

    if (animationTimer >= currentAnim.frameDuration) {
        animationTimer -= currentAnim.frameDuration; // Subtract to keep overflow for next frame check if needed
        currentFrame++;

        int frameCount = (currentAnim.type == Animation::AnimationType::SpriteSheet)
            ? currentAnim.frames.size()
            : currentAnim.textureFrames.size();

        if (currentFrame >= frameCount) {
            if (currentAnim.loop) {
                currentFrame = 0;
            }
            else {
                currentFrame = frameCount - 1;
                if (currentState >= EnemyState::Attacking && currentState <= EnemyState::Healing) {
                    setState(EnemyState::Idle); // Quay về Idle sau khi tấn công/hành động xong
                }
                if (currentState == EnemyState::Dying) {
                    isAlive = false; // Đánh dấu là đã chết hẳn sau khi animation kết thúc
                }
            }
        }

        if (currentAnim.type == Animation::AnimationType::SpriteSheet) {
            sprite.setTextureRect(currentAnim.frames[currentFrame]);
        }
        else { // TextureList
            sprite.setTexture(*currentAnim.textureFrames[currentFrame]);
        }
    }
}

void Enemy::setState(EnemyState newState) {

    if (currentState == newState) return;

    currentState = newState;
    currentFrame = 0;
    animationTimer = 0.f;

    if (animations.count(currentState)) {
        const Animation& anim = animations[currentState];
        if (anim.type == Animation::AnimationType::SpriteSheet && !anim.frames.empty()) {
            sprite.setTexture(*anim.sheet);
            sprite.setTextureRect(anim.frames[0]);
        }
        else if (anim.type == Animation::AnimationType::TextureList && !anim.textureFrames.empty()) {
            sprite.setTexture(*anim.textureFrames[0]);
        }
        adjustSpriteOriginAndScale();
    }
}

void Enemy::updateFacingDirection(const sf::Vector2f& playerPos) {
	// Add a dead zone to prevent jittering when player is very close
    const float deadZone = 10.0f; // 10 pixels
    float distanceX = playerPos.x - position.x;

    if (std::abs(distanceX) < deadZone) {
		return; // Do nothing if player is within dead zone
    }
    if (currentState == EnemyState::Attacking || currentState == EnemyState::Hurt) return;
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
    // Lấy kích thước của frame animation hiện tại
    sf::FloatRect frameBounds;
    if (animations.count(currentState)) {
        const auto& anim = animations.at(currentState);
        if (anim.type == Animation::AnimationType::SpriteSheet) {
            frameBounds = sf::FloatRect(sprite.getTextureRect());
        }
        else if (!anim.textureFrames.empty()) {
            frameBounds = sf::FloatRect(0, 0, anim.textureFrames[0]->getSize().x, anim.textureFrames[0]->getSize().y);
        }
    }
    // Đặt origin ở chính giữa-dưới của FRAME
    sprite.setOrigin(frameBounds.width / 2.0f, frameBounds.height);
    sprite.setScale(direction * this->baseScale, this->baseScale);
}


bool Enemy::isDead() const {
    return !isAlive && currentState == EnemyState::Dying; //&& currentFrame >= static_cast<int>(animations.at(EnemyState::Dying).frames.size()) - 1;
    // A more robust check for "completely finished dying and can be removed"
}

sf::Vector2f Enemy::getPosition() const {
    return position;
}

sf::FloatRect Enemy::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::FloatRect Enemy::getHitBox() const {
    //// Generic hitbox calculation, can be overridden by derived classes if needed
    return sprite.getTransform().transformRect(localHitbox);
}

bool Enemy::checkIsHurting() const {
    return isHurting;
}

EnemyState Enemy::getCurrentState() const {
    return currentState;
}

void Enemy::handlePhysicsAndCollision(float deltaTime, const CollisionLayer& collisionLayer) {
    const unsigned int TILE_SIZE = 32;

    // Áp dụng trọng lực
    velocity.y += GRAVITY * deltaTime;

    // --- Di chuyển và va chạm ngang ---
    position.x += velocity.x * deltaTime;
    sf::FloatRect enemyBounds = getHitBox();

    // Dùng vòng lặp để check nhiều điểm cho chính xác
    for (float y = enemyBounds.top; y < enemyBounds.top + enemyBounds.height; y += TILE_SIZE / 4.f) {
        if (velocity.x > 0 && collisionLayer.isCollidable(enemyBounds.left + enemyBounds.width, y)) {
            float offset = enemyBounds.left - position.x;
            position.x = (std::floor((enemyBounds.left + enemyBounds.width) / TILE_SIZE) * TILE_SIZE) - enemyBounds.width - offset - 0.1f;
            velocity.x = 0;
            break;
        }
        else if (velocity.x < 0 && collisionLayer.isCollidable(enemyBounds.left, y)) {
            float offset = enemyBounds.left - position.x;
            position.x = (std::floor(enemyBounds.left / TILE_SIZE) * TILE_SIZE + TILE_SIZE) - offset + 0.1f;
            velocity.x = 0;
            break;
        }
    }

    // --- Di chuyển và va chạm dọc ---
    position.y += velocity.y * deltaTime;
    enemyBounds = getHitBox();
    isOnGround = false;

    // Vòng lặp kiểm tra va chạm đất
    for (float x = enemyBounds.left; x < enemyBounds.left + enemyBounds.width; x += TILE_SIZE / 4.f) {
        if (velocity.y >= 0 && collisionLayer.isCollidable(x, enemyBounds.top + enemyBounds.height)) {
            float offset = enemyBounds.top - position.y;
            float newY = (std::floor((enemyBounds.top + enemyBounds.height) / TILE_SIZE) * TILE_SIZE) - enemyBounds.height - offset;
            position.y = newY;
            velocity.y = 0;
            isOnGround = true;
            break;
        }
    }

    // Kiểm tra va chạm trần nhà (nếu cần)
    if (velocity.y < 0) {
        if (collisionLayer.isCollidable(enemyBounds.left, enemyBounds.top) || collisionLayer.isCollidable(enemyBounds.left + enemyBounds.width, enemyBounds.top)) {
            float offset = enemyBounds.top - position.y;
            position.y = (std::floor(enemyBounds.top / TILE_SIZE) * TILE_SIZE + TILE_SIZE) - offset;
            velocity.y = 0;
        }
    }

}