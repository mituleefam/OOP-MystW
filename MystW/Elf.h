#pragma once
#include "Enemy.h"
#include "AnimatedProjectile.h" // For Elf's arrows
#include <vector>
#include <map>

//// Forward declare AnimSheetInfo if it's not in a common header
//struct AnimSheetInfo {
//    const char* filename;
//    int frameWidth;
//    int frameHeight;
//    int frameCount;
//    float frameDuration;
//    bool loop;
//};

class Elf : public Enemy {
public:
    Elf(const std::string& assetBaseFolder, float startX, float startY);
    ~Elf() override = default;

    // Override virtual functions from base Enemy class
    void loadSpecificAssets() override;
    //void performAttackLogic(const sf::Vector2f& playerPos) override;
    void performAttackLogic(Player& player) override;
    void update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) override;
    void updateAI(float deltaTime, Player& player) override;
    void draw(sf::RenderWindow& window) override; // To draw arrows

    sf::FloatRect getHitBox() const override;

    // Elf-specific methods for managing arrows (if needed publicly, or keep private/protected)
    bool checkArrowCollisions(const sf::FloatRect& playerBounds);
    void removeArrowsCollidingWith(const sf::FloatRect& playerBounds);

private:
    std::string baseAssetPath; // To store the path like "Assets/Enemy/Elf/Textures"

    // Elf-specific animation data
    static const std::map<EnemyState, AnimSheetInfo> elfAnimSheetInfos;
    void loadStateFrames(EnemyState state, const AnimSheetInfo& info); // Helper

    // Arrow related members
    std::vector<AnimatedProjectile> arrows;
    std::shared_ptr<sf::Texture> arrowTexture;
    sf::Clock shootCooldownTimer;
    float shootInterval; // How often the Elf can shoot
    float attackRange;   // Range within which Elf decides to attack

    // Elf-specific methods
    //void tryShootArrow(const sf::Vector2f& playerPos);
    void tryShootArrow(const sf::Vector2f& playerPos, const sf::FloatRect& playerHitBox);
    void updateArrows(float deltaTime, const sf::RenderWindow& window); // Pass window for bounds checks
};