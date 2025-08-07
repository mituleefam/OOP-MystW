#pragma once
#include "Enemy.h"
#include "SpellEffect.h"
#include <vector>

class Wizard : public Enemy
{
public:
    Wizard(const std::string& assetBaseFolder, float startX, float startY);
    ~Wizard() override = default;

    // Override virtual functions from base Enemy class
    void loadSpecificAssets() override;
    void performAttackLogic(Player& player) override;
    void update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) override;
    void updateAI(float deltaTime, Player& player) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getHitBox() const override;

    // Wizard-specific methods
    //bool attackRegistered = false;
    //bool checkSpellCollisions(const sf::FloatRect& playerHitBox) const;
    //void onSpellHit();
private:
    std::string baseAssetPath; // To store the path like "Assets/Enemy/Wizard/Textures"

    // Wizard-specific animation data
    static const std::map<EnemyState, AnimSheetInfo> wizardAnimSheetInfos;
    void loadStateFrames(EnemyState state, const AnimSheetInfo& info); // Helper
    // void adjustSpriteOriginAndScale() override;

    sf::Clock attackCooldownTimer;
    float spellInterval; // How often the Wizard can attack
    float spellRange;   // Range within which Wizard decides to attack

    float meleeInterval; // Interval for melee attacks
    float meleeRange; // Range for melee attacks

    float detectionRange; // Range within which the Wizard detects the player
    // Attack logic flags
    bool attackActionTriggered;     // To trigger an action (spell/hit) only once per animation
    bool damageDealtInCurAttack;    // For melee, to ensure damage is dealt only on the hit frame
    //std::vector<SpellEffect> lightningStrikes; // for multiple strikes
    std::unique_ptr<SpellEffect> lightningStrike; // Single strike for simplicity
    sf::Texture lightningTexture;

    // Spell animation data
    sf::Vector2i spellFrameSize;
    int spellFrameCount;
    float spellAnimDuration;
};
