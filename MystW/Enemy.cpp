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
    hurtDuration(0.7f) // Default hurt duration, can be overridden
{
    sprite.setPosition(position);
    // Derived class will call loadSpecificAssets() and then likely setState(EnemyState::Idle)
}

void Enemy::update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) {
    if (!isAlive) {
        return;
    }

    // Dying state handling
    if (currentState == EnemyState::Dying) {
        animate(deltaTime);
        //const Animation& anim = animations[currentState];
        //// Check if dying animation has finished
        //if (!anim.frames.empty() && !anim.loop && currentFrame >= static_cast<int>(anim.frames.size()) - 1) {
        //    isAlive = false; // Now truly dead and won't be updated/drawn further (handled in main loop)
        //}
        return; // No other logic if dying
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
    //if (!isAlive && currentState != EnemyState::Dying) { // Don't draw if dead, unless it's the dying animation itself
    //    return;
    //}
    //if (currentState == EnemyState::Dying && !animations[EnemyState::Dying].frames.empty() && currentFrame >= static_cast<int>(animations[EnemyState::Dying].frames.size()) - 1) {
    //    // If dying animation is finished, and isAlive is already set to false, don't draw.
    //    if (!isAlive) return;
    //}
    //window.draw(sprite);
    if (isAlive || currentState == EnemyState::Dying) {
        window.draw(sprite);
    }
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

        if (currentFrame >= frameCount) {//static_cast<int>(currentAnim.frames.size())) {
            if (currentAnim.loop) {
                currentFrame = 0;
            }
            else {
                currentFrame = frameCount - 1;//static_cast<int>(currentAnim.frames.size()) - 1; // Stay on last frame
                //if (currentState == EnemyState::Attacking || currentState == EnemyState::Attacking2) { // Example: transition from attacking to idle
                //    setState(EnemyState::Idle);
                //}
                if (currentState >= EnemyState::Attacking && currentState <= EnemyState::Healing) {
                    setState(EnemyState::Idle); // Quay về Idle sau khi tấn công/hành động xong
                }
                if (currentState == EnemyState::Dying) {
                    isAlive = false; // Đánh dấu là đã chết hẳn sau khi animation kết thúc
                }
            }
        }
        //sprite.setTextureRect(currentAnim.frames[currentFrame]);
        if (currentAnim.type == Animation::AnimationType::SpriteSheet) {
            sprite.setTextureRect(currentAnim.frames[currentFrame]);
        }
        else { // TextureList
            sprite.setTexture(*currentAnim.textureFrames[currentFrame]);
        }
    }
}

void Enemy::setState(EnemyState newState) {
    //if (currentState != newState || (newState == EnemyState::Hurt && !isHurting) || (newState == EnemyState::Attacking && currentState != EnemyState::Attacking)) {
    //    // Allow re-triggering attack/hurt if not already in it
    //    currentState = newState;
    //    currentFrame = 0;
    //    animationTimer = 0.f;

    //    if (animations.count(currentState) && !animations[currentState].frames.empty()) {
    //        sprite.setTexture(*animations[currentState].sheet); // Ensure correct sheet is set
    //        sprite.setTextureRect(animations[currentState].frames[0]);
    //        adjustSpriteOriginAndScale(); // Adjust origin for new frame size potentially
    //    }
    //    else {
    //        // std::cerr << "Warning: No animation frames for state " << static_cast<int>(newState) << std::endl;
    //    }
    //}
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
    //const sf::Texture* currentTexture = sprite.getTexture();
    //if (!currentTexture) return; // Nếu không có texture thì không làm gì cả

    //// Lấy kích thước của TOÀN BỘ file ảnh texture hiện tại
    //sf::Vector2u textureSize = currentTexture->getSize();

    //// Lấy textureRect để xử lý trường hợp sprite sheet
    //sf::IntRect textureRect = sprite.getTextureRect();

    //// Nếu textureRect có kích thước (ví dụ: sprite sheet), dùng nó.
    //// Nếu không (ví dụ: boss dùng cả file ảnh), dùng kích thước của cả texture.
    //float width = (textureRect.width != 0) ? textureRect.width : textureSize.x;
    //float height = (textureRect.height != 0) ? textureRect.height : textureSize.y;

    //// Đặt origin ở chính giữa-đáy của hình ảnh thực tế
    //sprite.setOrigin(width / 2.0f, height);
    //sprite.setScale(direction * this->baseScale, this->baseScale);
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
    //sf::FloatRect box = sprite.getGlobalBounds();
    //float shrinkHorizontal = 0.6f; // Make hitbox 60% of sprite width
    //float shrinkVertical = 0.6f;   // Make hitbox 60% of sprite height

    //box.left += box.width * (1.0f - shrinkHorizontal) / 2.0f;
    //box.width *= shrinkHorizontal;
    //box.top += box.height * (1.0f - shrinkVertical) / 2.0f;
    //box.height *= shrinkVertical;
    //return box;
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
	// BELOW CODE ENEMY IS STUCK, NOT WORKING PROPERLY
    //const unsigned int TILE_SIZE = 32;

    //// 1. Áp dụng trọng lực
    //velocity.y += GRAVITY * deltaTime;

    //// 2. Xử lý va chạm theo trục X
    //position.x += velocity.x * deltaTime;
    //sf::FloatRect bounds = getHitBox();

    //if (velocity.x > 0) { // Di chuyển sang phải
    //    if (collisionLayer.isCollidable(bounds.left + bounds.width, bounds.top) ||
    //        collisionLayer.isCollidable(bounds.left + bounds.width, bounds.top + bounds.height / 2) ||
    //        collisionLayer.isCollidable(bounds.left + bounds.width, bounds.top + bounds.height))
    //    {
    //        position.x = (std::floor((bounds.left + bounds.width) / TILE_SIZE) * TILE_SIZE) - (bounds.width + (bounds.left - position.x)) - 0.1f;
    //        velocity.x = 0;
    //    }
    //}
    //else if (velocity.x < 0) { // Di chuyển sang trái
    //    if (collisionLayer.isCollidable(bounds.left, bounds.top) ||
    //        collisionLayer.isCollidable(bounds.left, bounds.top + bounds.height / 2) ||
    //        collisionLayer.isCollidable(bounds.left, bounds.top + bounds.height))
    //    {
    //        position.x = (std::floor(bounds.left / TILE_SIZE) * TILE_SIZE + TILE_SIZE) - (bounds.left - position.x) + 0.1f;
    //        velocity.x = 0;
    //    }
    //}

    //// 3. Xử lý va chạm theo trục Y
    //position.y += velocity.y * deltaTime;
    //bounds = getHitBox(); // Lấy lại hitbox sau khi di chuyển
    //isOnGround = false;

    //if (velocity.y > 0) { // Rơi xuống
    //    if (collisionLayer.isCollidable(bounds.left, bounds.top + bounds.height) ||
    //        collisionLayer.isCollidable(bounds.left + bounds.width, bounds.top + bounds.height))
    //    {
    //        position.y = (std::floor((bounds.top + bounds.height) / TILE_SIZE) * TILE_SIZE) - (bounds.height + (bounds.top - position.y));
    //        velocity.y = 0;
    //        isOnGround = true;
    //    }
    //}
    //else if (velocity.y < 0) { // Nhảy lên
    //    if (collisionLayer.isCollidable(bounds.left, bounds.top) ||
    //        collisionLayer.isCollidable(bounds.left + bounds.width, bounds.top))
    //    {
    //        position.y = (std::floor(bounds.top / TILE_SIZE) * TILE_SIZE + TILE_SIZE) - (bounds.top - position.y);
    //        velocity.y = 0;
    //    }
    //}
}