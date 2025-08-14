#include "LeafBoss.h"
#include "Player.h"
#include "Math.h"
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

LeafBoss::LeafBoss(const std::string& assetFolder, float startX, float startY)
    : Enemy(startX, startY, 1, 110.0f), // postion (x,y), health, speed
    baseAssetPath(assetFolder),
    currentAttackType(LeafAttackType::None),
    actionTriggeredInState(false),
    isDefending(false),
    isRolling(false),
	hitsTakenSinceAction(0)
{
    baseScale = 3.0f;
    hurtDuration = 0.5f;
    loadSpecificAssets();
    setState(EnemyState::Idle);
    attackCooldown.restart();
    specialMoveClock.restart();
}

void LeafBoss::loadSpecificAssets() {
    loadAnimationFrames("Idle", EnemyState::Idle);
    loadAnimationFrames("Run", EnemyState::Running);
    loadAnimationFrames("Attack1", EnemyState::Attacking);
    loadAnimationFrames("Attack2", EnemyState::Attacking2);
    loadAnimationFrames("Attack3", EnemyState::Attacking3);
    loadAnimationFrames("Hurt", EnemyState::Hurt);
    loadAnimationFrames("Die", EnemyState::Dying);
    loadAnimationFrames("Defend", EnemyState::Defending);
    loadAnimationFrames("Roll", EnemyState::Rolling);

    loadProjectileAssets();

    // --- HƯỚNG DẪN CHỈNH HITBOX CHO LEAFBOSS ---
    // Dựa trên kích thước thật của file ảnh animation (bạn có thể xem trong properties của file)
    const float FRAME_WIDTH = 288.f;  // Chiều rộng của một frame animation
    const float FRAME_HEIGHT = 128.f; // Chiều cao của một frame animation

    // 1. CHỈNH KÍCH THƯỚC BOX: Thay đổi hai giá trị này
    float hitboxWidth = 50.f;  // Độ rộng của hitbox (làm cho nó nhỏ hơn)
    float hitboxHeight = 50.f; // Độ cao của hitbox

    // 2. CHỈNH VỊ TRÍ BOX (Thường không cần sửa)
    // Các dòng dưới đây sẽ tự động đặt hitbox ở chính giữa-đáy của frame
    float hitboxLeft = (FRAME_WIDTH - hitboxWidth) / 2.0f;
    float hitboxTop = FRAME_HEIGHT - hitboxHeight;

    localHitbox = sf::FloatRect(hitboxLeft, hitboxTop, hitboxWidth, hitboxHeight);
}

void LeafBoss::loadAnimationFrames(const std::string& stateName, EnemyState state) {
    std::string path = baseAssetPath + "/" + stateName;
    if (!fs::exists(path)) {
        std::cerr << "Path does not exist: " << path << std::endl;
        return;
    }

    Animation anim;
    anim.frameDuration = 0.12f;
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

void LeafBoss::loadProjectileAssets() {
    std::string rainPath = baseAssetPath + "/Projectile/Rain";
    if (fs::exists(rainPath)) {
        std::vector<std::shared_ptr<sf::Texture>> textures;
        for (const auto& entry : fs::directory_iterator(rainPath)) {
            auto tex = std::make_shared<sf::Texture>();
            if (tex->loadFromFile(entry.path().string())) {
                textures.push_back(tex);
            }
        }
        projectileTextures[LeafAttackType::Rain] = textures;
    }
    else {
        std::cerr << "Rain projectile texture path not found: " << rainPath << std::endl;
    }
    std::string entanglePath = baseAssetPath + "/Projectile/Entangle";
    if (fs::exists(entanglePath)) {
        projectileTextures[LeafAttackType::EntangleArrow] = {}; // Tạo entry
        for (const auto& entry : fs::directory_iterator(entanglePath)) {
            auto tex = std::make_shared<sf::Texture>();
            if (tex->loadFromFile(entry.path().string())) {
                projectileTextures[LeafAttackType::EntangleArrow].push_back(tex);
            }
        }
    }
    else {
        std::cerr << "Entangle projectile texture path not found: " << entanglePath << std::endl;
	}
}

void LeafBoss::update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) {
    for (auto& proj : projectiles) {
        proj.update(deltaTime);
        if (!proj.isDead() && !proj.hasDealtDamage() && proj.getHitBox().intersects(player.getHitBox())) {
            player.takeDamage(getAttackDamage(proj.getType()));
            proj.setDamageDealt(true);
        }
    }
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const LeafProjectile& p) { return p.isDead(); }), projectiles.end());

    Enemy::update(deltaTime, player, collisionLayer);
}

void LeafBoss::updateAI(float deltaTime, Player& player) {
    if (player.isDead()) {
        setState(EnemyState::Idle);
        velocity.x = 0;
        return;
    }
    if (isHurting || !isAlive || currentState == EnemyState::Dying) return;

    updateFacingDirection(player.getPosition());
    float distanceToPlayer = magnitude(player.getPosition() - position);

    if (isDefending || isRolling) {
		velocity.x = isRolling ? (-direction * speed * 2.0f) : 0; // Dodge player
        if (actionTimer.getElapsedTime().asSeconds() > 1.5f) {
            isDefending = isRolling = false;
            setState(EnemyState::Idle);
        }
        return;
    }
    if (currentState >= EnemyState::Attacking && currentState <= EnemyState::Attacking5) {
        velocity.x = 0;
        performAttackLogic(player);
        return;
    }

    actionTriggeredInState = false;

    bool canAttack = attackCooldown.getElapsedTime().asSeconds() > 2.0f;

    if (canAttack) {
        if (distanceToPlayer < 150.f) {
            setState(EnemyState::Attacking);
            currentAttackType = LeafAttackType::Melee;
        }
        else if (distanceToPlayer < 700.f) {
            setState(EnemyState::Attacking2);
            currentAttackType = LeafAttackType::EntangleArrow;
        }
        else {
            setState(EnemyState::Attacking3);
            currentAttackType = LeafAttackType::Rain;
        }
        attackCooldown.restart();
    }
    else if (distanceToPlayer > 180.f) {
        setState(EnemyState::Running);
        velocity.x = direction * speed;
    }
    else {
        setState(EnemyState::Idle);
        velocity.x = 0;
    }
}

void LeafBoss::performAttackLogic(Player& player) {

    if (actionTriggeredInState) return;
    lastPlayerTargetPos = player.getPosition(); // Luôn cập nhật vị trí player

    if (currentState == EnemyState::Attacking) {
        if (currentFrame == 3) {
            if (getHitBox().intersects(player.getHitBox())) {
                player.takeDamage(getAttackDamage(LeafAttackType::Melee));
            }
            actionTriggeredInState = true;
        }
    }
    else if (currentState == EnemyState::Attacking2 || currentState == EnemyState::Attacking3) {
        if (currentFrame == 5) {
            shootProjectile(currentAttackType, lastPlayerTargetPos);
            actionTriggeredInState = true;
        }
    }
}

void LeafBoss::animate(float deltaTime) {
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
                else if (currentState >= EnemyState::Attacking && currentState <= EnemyState::Rolling) {
                    setState(EnemyState::Idle);
                }
            }
        }
        sprite.setTexture(*anim.textureFrames[currentFrame], true);
    }
}

void LeafBoss::draw(sf::RenderWindow& window) {
    Enemy::draw(window);
    for (const auto& proj : projectiles) {
        proj.draw(window);
    }
}

void LeafBoss::takeDamage(int damageAmount) {
    if (isDefending) {
        damageAmount /= 2;
    }
    Enemy::takeDamage(damageAmount);
    // *** AI THÔNG MINH HƠN: Chỉ Roll/Defend sau khi bị đánh ***
    if (isAlive && !isRolling && !isDefending) {
        hitsTakenSinceAction++;
        if (hitsTakenSinceAction >= 3 && specialMoveClock.getElapsedTime().asSeconds() > 5.0f) {
            setState(rand() % 2 == 0 ? EnemyState::Defending : EnemyState::Rolling);
            isDefending = (currentState == EnemyState::Defending);
            isRolling = (currentState == EnemyState::Rolling);
            actionTimer.restart();
            specialMoveClock.restart();
            hitsTakenSinceAction = 0;
        }
    }
}

void LeafBoss::shootProjectile(LeafAttackType type, const sf::Vector2f& target) {
    if (projectileTextures.find(type) == projectileTextures.end() || projectileTextures[type].empty()) {
        std::cerr << "No textures loaded for projectile type: " << static_cast<int>(type) << std::endl;
        return;
    }

    if (type == LeafAttackType::Rain) {
        // Tạo ra 5 viên đạn mưa rơi xuống quanh vị trí của player
        for (int i = -2; i <= 2; ++i) {
            sf::Vector2f spawnPos = target;
            spawnPos.x += i * 50.f; // Rải đạn ra theo chiều ngang
            spawnPos.y -= 500.f;    // Xuất hiện từ trên cao

            sf::Vector2f velocity(0.f, 400.f); // Rơi thẳng xuống

            projectiles.emplace_back(projectileTextures[type], spawnPos, velocity, 0.1f, type, 5.0f);
            projectiles.back().setTargetY(target.y);
        }
        std::cout << "LeafBoss created rain effect!" << std::endl;
    }
    else if (type == LeafAttackType::EntangleArrow) {
        sf::Vector2f spawnPos = this->position;
        spawnPos.y -= 50; // Điều chỉnh vị trí bắn ra
        sf::Vector2f shootDir = target - spawnPos;
        normalizeVector(shootDir);
        float projectileSpeed = 500.f;
        sf::Vector2f velocity = shootDir * projectileSpeed;
        projectiles.emplace_back(projectileTextures[type], spawnPos, velocity, 0.1f, type, 3.0f);
    }
    // Thêm logic cho các loại đạn khác ở đây nếu cần
}

int LeafBoss::getAttackDamage(LeafAttackType type) const {
    // Trả về sát thương dựa trên loại tấn công
    switch (type) {
    case LeafAttackType::Melee:
        return 5;

    case LeafAttackType::EntangleArrow:
        return 4;

    case LeafAttackType::Rain:
        return 3;
    default:
        return 2; 
    }
}
