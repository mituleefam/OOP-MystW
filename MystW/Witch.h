#pragma once
#include "Enemy.h"
#include "AnimatedProjectile.h" // For Witch's arrows
#include <vector>
#include <map>

class Witch : public Enemy {
public:
    Witch(const std::string& assetBaseFolder, float startX, float startY);
    ~Witch() override = default;

    // Override virtual functions from base Enemy class
    void loadSpecificAssets() override;
    void performAttackLogic(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) override;
    void update(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) override;
    void updateAI(float deltaTime, const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox) override;
    void draw(sf::RenderWindow& window) override; // To draw arrows

    // Witch-specific methods for managing "arrows" (if needed publicly, or keep private/protected)
    bool checkArrowCollisions(const sf::FloatRect& playerBounds);
    void removeArrowsCollidingWith(const sf::FloatRect& playerBounds);

private:
    std::string baseAssetPath; // To store the path like "Assets/Enemy/Witch/Textures"

    // Witch-specific animation data
    static const std::map<EnemyState, AnimSheetInfo> witchAnimSheetInfos;
    void loadStateFrames(EnemyState state, const AnimSheetInfo& info); // Helper

    // Arrow related members
    std::vector<AnimatedProjectile> arrows;
    std::shared_ptr<sf::Texture> arrowTexture;
    sf::Clock shootCooldownTimer;
    float shootInterval; // How often the Witch can shoot
    float attackRange;   // Range within which Witch decides to attack

    // Witch-specific methods
    void tryShootArrow(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox);
    void updateArrows(float deltaTime, const sf::RenderWindow& window); // Pass window for bounds checks
};

