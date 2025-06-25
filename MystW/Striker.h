#pragma once
#include "Enemy.h"

//// Forward declare AnimSheetInfo if it's not in a common header
//struct AnimSheetInfo {
//    const char* filename;
//    int frameWidth;
//    int frameHeight;
//    int frameCount;
//    float frameDuration;
//    bool loop;
//};

class Striker : public Enemy
{
public:
    Striker(const std::string& assetBaseFolder, float startX, float startY);
    ~Striker() override = default;

    // Override virtual functions from base Enemy class
    void loadSpecificAssets() override;
    //void performAttackLogic(const sf::Vector2f& playerPos) override;
    void performAttackLogic(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) override;
    void update(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) override;
    void updateAI(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) override;
    void draw(sf::RenderWindow& window) override;

	// Striker-specific methods
    //bool checkAttackCollisions(const sf::FloatRect& playerHitBox) const;
    bool attackRegistered = false;

private:
    std::string baseAssetPath; // To store the path like "Assets/Enemy/Striker/Textures"

    // Striker-specific animation data
    static const std::map<EnemyState, AnimSheetInfo> strikerAnimSheetInfos;
    void loadStateFrames(EnemyState state, const AnimSheetInfo& info); // Helper

    sf::Clock attackCooldownTimer;
    float attackInterval; // How often the Striker can attack
    float attackRange;   // Range within which Striker decides to attack
};

