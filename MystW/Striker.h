#pragma once
#include "Enemy.h"

class Striker : public Enemy
{
public:
    Striker(const std::string& assetBaseFolder, float startX, float startY);
    ~Striker() override = default;

    // Override virtual functions from base Enemy class
    void loadSpecificAssets() override;
	void performAttackLogic(Player& player) override;
	void update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) override;
    void updateAI(float deltaTime, Player& player) override;
    void draw(sf::RenderWindow& window) override;

    sf::FloatRect getHitBox() const override;

	// Striker-specific methods
    bool attackRegistered = false;
    bool damageDealtInCurAttack = false; // << THÊM BIẾN CỜ NÀY

private:
    std::string baseAssetPath; // To store the path like "Assets/Enemy/Striker/Textures"

    // Striker-specific animation data
    static const std::map<EnemyState, AnimSheetInfo> strikerAnimSheetInfos;
    void loadStateFrames(EnemyState state, const AnimSheetInfo& info); // Helper

    sf::Clock attackCooldownTimer;
    float attackInterval; // How often the Striker can attack
    float attackRange;   // Range within which Striker decides to attack
};

