#pragma once
#include "Enemy.h"
#include "LeafProjectile.h"
#include <map>
#include <vector>
#include <memory>

class LeafBoss : public Enemy {
public:
    LeafBoss(const std::string& assetFolder, float startX, float startY);
    ~LeafBoss() override = default;

    // --- Các hàm được override từ lớp Enemy ---
    void loadSpecificAssets() override;
    void update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) override;
    void updateAI(float deltaTime, Player& player) override;
    void performAttackLogic(Player& player) override;
    void animate(float deltaTime) override; // Boss dùng hệ thống animation riêng
    void draw(sf::RenderWindow& window) override;
    void takeDamage(int damageAmount) override;

private:
    // Nạp tài nguyên
    void loadAnimationFrames(const std::string& stateName, EnemyState state);
    void loadProjectileAssets();

    // Logic tấn công
    void shootProjectile(LeafAttackType type, const sf::Vector2f& target);
    int getAttackDamage(LeafAttackType type) const;

    // --- Biến thành viên ---
    std::string baseAssetPath;

    // Đạn
    std::map<LeafAttackType, std::vector<std::shared_ptr<sf::Texture>>> projectileTextures;
    std::vector<LeafProjectile> projectiles;

    // Trạng thái & Đồng hồ
    sf::Clock attackCooldown;
    sf::Clock specialMoveClock;
    sf::Clock actionTimer;

    LeafAttackType currentAttackType;
    sf::Vector2f lastPlayerTargetPos;

    // Cờ cho các hành vi phức tạp
    bool actionTriggeredInState;
    bool isDefending;
    bool isRolling;

    // --- BIẾN MỚI CHO AI THÔNG MINH HƠN ---
    int hitsTakenSinceAction;         // Đếm số lần bị đánh
    const int rollHitThreshold = 2;   // Sẽ Roll sau khi bị đánh 3 lần
};