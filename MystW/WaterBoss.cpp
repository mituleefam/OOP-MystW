#include "WaterBoss.h"
#include "Player.h"
#include "Math.h"
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

WaterBoss::WaterBoss(const std::string& assetFolder, float startX, float startY)
	: Enemy(startX, startY, 1, 200.0f), // postion (x,y), health, speed
    baseAssetPath(assetFolder),
    damageDealtInAttack(false),
    isDefending(false),
    isRolling(false),
    isHealing(false),
    hitsTakenSinceAction(0)
{
    baseScale = 4.0f;
    hurtDuration = 0.4f;
    loadSpecificAssets();
    setState(EnemyState::Idle);
    specialCooldown.restart();
}

void WaterBoss::loadSpecificAssets() {
    loadAnimationFrames("Idle", EnemyState::Idle);
    loadAnimationFrames("Run", EnemyState::Running);
    loadAnimationFrames("Attack1", EnemyState::Attacking);
    loadAnimationFrames("Attack2", EnemyState::Attacking2);
    loadAnimationFrames("Attack3", EnemyState::Attacking3);
    loadAnimationFrames("Hurt", EnemyState::Hurt);
    loadAnimationFrames("Die", EnemyState::Dying);
    loadAnimationFrames("Defend", EnemyState::Defending);
    loadAnimationFrames("Roll", EnemyState::Rolling);
    loadAnimationFrames("Heal", EnemyState::Healing);

    // --- HƯỚNG DẪN CHỈNH HITBOX CHO WATERBOSS ---
    // Dựa trên kích thước thật của file ảnh animation
    const float FRAME_WIDTH = 288.f;  // Chiều rộng của một frame
    const float FRAME_HEIGHT = 128.f; // Chiều cao của một frame

    // 1. CHỈNH KÍCH THƯỚC BOX:
    float hitboxWidth = 60.f;   // Độ rộng mong muốn
    float hitboxHeight = 40.f; // Độ cao mong muốn

    // 2. TÍNH TOÁN VỊ TRÍ:
    float hitboxLeft = (FRAME_WIDTH - hitboxWidth) / 2.0f;
    float hitboxTop = FRAME_HEIGHT - hitboxHeight;

    localHitbox = sf::FloatRect(hitboxLeft, hitboxTop, hitboxWidth, hitboxHeight);
}

void WaterBoss::loadAnimationFrames(const std::string& stateName, EnemyState state) {
    std::string path = baseAssetPath + "/" + stateName;
    if (!fs::exists(path)) {
        std::cerr << "Path does not exist for WaterBoss animation: " << path << std::endl;
        return;
    }

    Animation anim;
    anim.frameDuration = 0.1f;
    anim.loop = (state == EnemyState::Idle || state == EnemyState::Running);
    anim.type = Animation::AnimationType::TextureList;

    std::vector<fs::directory_entry> entries(fs::directory_iterator(path), {});
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.path().string() < b.path().string();
        });

    for (const auto& entry : entries) {
        auto tex = std::make_shared<sf::Texture>();
        if (tex->loadFromFile(entry.path().string())) {
            anim.textureFrames.push_back(tex);
        }
    }
    animations[state] = anim;
}

void WaterBoss::updateAI(float deltaTime, Player& player) {
    if (player.isDead()) {
        setState(EnemyState::Idle);
        velocity.x = 0;
        return;
    }

    const float VERTICAL_TOLERANCE = 50.0f;
    sf::FloatRect playerHitBox = player.getHitBox();
    sf::FloatRect enemyHitBox = getHitBox();
    float playerBottom = playerHitBox.top + playerHitBox.height;
    float enemyBottom = enemyHitBox.top + enemyHitBox.height;

    if (std::abs(playerBottom - enemyBottom) > VERTICAL_TOLERANCE) {
        // Nếu người chơi ở tầng khác, boss sẽ đứng im
        setState(EnemyState::Idle);
        velocity.x = 0;
        return; // Không xử lý AI gì thêm
    }

    if (isHurting || !isAlive || currentState == EnemyState::Dying) return;

    updateFacingDirection(player.getPosition());
    float distanceToPlayer = magnitude(player.getPosition() - position);

    // Nếu đang Roll, để nó hoàn thành
    if (isRolling) {
        velocity.x = (-direction * speed * 1.5f); // Lăn ra xa
        if (actionTimer.getElapsedTime().asSeconds() > 1.f) { // Giảm thời gian lăn
            isRolling = false;
            setState(EnemyState::Idle);
        }
        return;
    }
    // Nếu đang tấn công, để nó hoàn thành
    if (currentState >= EnemyState::Attacking && currentState <= EnemyState::Attacking3) {
        velocity.x = 0;
        performAttackLogic(player);
        return;
    }

    damageDealtInAttack = false;

    // Có cơ hội Roll chủ động để áp sát
    bool canUseSpecial = specialCooldown.getElapsedTime().asSeconds() > 4.0f; // Cooldown cho các hành động đặc biệt
    if (distanceToPlayer < 250.f && distanceToPlayer > 50.f && canUseSpecial) {
        if (rand() % 4 == 0) { // 25% cơ hội
            setState(EnemyState::Rolling);
            isRolling = true;
            velocity.x = direction * speed * 2.0f; // Lăn VỀ PHÍA player
            actionTimer.restart();
            specialCooldown.restart();
            hitsTakenSinceAction = 0;
            return; // Thực hiện Roll và bỏ qua các logic khác trong frame này
        }
    }

    // Quyết định hành động chỉ dựa trên khoảng cách
    if (distanceToPlayer < 180.f) {
        // Nếu đủ gần, tấn công
        int attackChoice = rand() % 3;
        if (attackChoice == 0) setState(EnemyState::Attacking);
        else if (attackChoice == 1) setState(EnemyState::Attacking2);
        else setState(EnemyState::Attacking3);
    }
    else if (distanceToPlayer < 1200.f) {
        // Nếu ở xa hơn, đuổi theo
        setState(EnemyState::Running);
        velocity.x = direction * speed;
    }
    else {
        // Nếu quá xa, đứng im
        setState(EnemyState::Idle);
        velocity.x = 0;
    }
}

void WaterBoss::performAttackLogic(Player& player) {
    if (damageDealtInAttack) return;
    switch (currentState) {
    case EnemyState::Attacking:
        if (currentFrame == 4) {
            if (getHitBox().intersects(player.getHitBox())) {
                player.takeDamage(5);
                damageDealtInAttack = true;
            }
        }
        break;
    case EnemyState::Attacking2:
        if (currentFrame == 5) {
            if (getHitBox().intersects(player.getHitBox())) {
                player.takeDamage(5);
                damageDealtInAttack = true;
            }
        }
        break;
    case EnemyState::Attacking3:
        if (currentFrame == 4) {
            if (getHitBox().intersects(player.getHitBox())) {
                player.takeDamage(5);
                damageDealtInAttack = true;
            }
        }
        break;
    default:
        break;
    }
}

void WaterBoss::animate(float deltaTime) {
    if (animations.find(currentState) == animations.end()) return;
    const Animation& anim = animations[currentState];
    if (anim.textureFrames.empty()) return;

    animationTimer += deltaTime;

    if (animationTimer >= anim.frameDuration) {
        animationTimer -= anim.frameDuration;
        currentFrame++;

        if (currentFrame >= anim.textureFrames.size()) {
            if (anim.loop) {
                currentFrame = 0;
            }
            else {
                currentFrame = anim.textureFrames.size() - 1;
                if (currentState == EnemyState::Dying) {
                    isAlive = false;
                }
                else if (currentState >= EnemyState::Attacking && currentState <= EnemyState::Healing) {
                    setState(EnemyState::Idle);
                }
            }
        }
        sprite.setTexture(*anim.textureFrames[currentFrame], true);
    }
}

void WaterBoss::takeDamage(int damageAmount) {
    if (isDefending) {
        damageAmount /= 3;
    }
    Enemy::takeDamage(damageAmount); // Gọi hàm gốc để trừ máu và vào trạng thái Hurt

    // Logic Roll thông minh hơn
    if (isAlive && !isRolling && !isDefending) {
        hitsTakenSinceAction++;
        if (hitsTakenSinceAction >= rollHitThreshold && specialCooldown.getElapsedTime().asSeconds() > 5.0f) {
            setState(EnemyState::Rolling);
            isRolling = true;
            actionTimer.restart();
            specialCooldown.restart();
            hitsTakenSinceAction = 0;
        }
    }
}
