#pragma once
#include "Enemy.h"
#include <map>
#include <vector>
#include <memory>

class WaterBoss : public Enemy {
public:
    WaterBoss(const std::string& assetFolder, float startX, float startY);
    ~WaterBoss() override = default;

    // --- Overridden virtual functions ---
    void loadSpecificAssets() override;
    void updateAI(float deltaTime, Player& player) override;
    void performAttackLogic(Player& player) override;
    void animate(float deltaTime) override;
    void takeDamage(int damageAmount) override; // Override this to add new logic

private:
    void loadAnimationFrames(const std::string& stateName, EnemyState state);

    // --- Member Variables ---
    std::string baseAssetPath;

    // State & Timers
    sf::Clock actionTimer;
    sf::Clock specialCooldown;

    // Flags for complex behaviors
    bool damageDealtInAttack;
    bool isDefending;
    bool isRolling;
    bool isHealing;

    // --- BIẾN MỚI CHO AI THÔNG MINH HƠN ---
    int hitsTakenSinceAction;         // Đếm số lần bị đánh
    const int rollHitThreshold = 3;   // Sẽ Roll sau khi bị đánh 3 lần
};
