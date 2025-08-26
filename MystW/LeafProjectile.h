#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

// Enum for different attack patterns of the Leaf Boss
enum class LeafAttackType {
    None,
    Melee,
    PoisonArrow,
    EntangleArrow,
    Rain,
    Impact,
    Downward,
    Beam
};

class LeafProjectile {
public:
    LeafProjectile(const std::vector<std::shared_ptr<sf::Texture>>& frames,
        const sf::Vector2f& spawnPos,
        const sf::Vector2f& velocity,
        float frameDuration,
        LeafAttackType type,
        float lifetime);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;

    // Getters
    bool isDead() const { return !alive; }
    LeafAttackType getType() const { return type; }
    sf::FloatRect getHitBox() const;

    // Damage tracking
    bool hasDealtDamage() const { return damageDealt; }
    void setDamageDealt(bool dealt) { damageDealt = dealt; }

    // Special mechanics for Rain attack
    void setTargetY(float y) { targetY = y; hasTargetY = true; }
    void setImpactFrames(const std::vector<std::shared_ptr<sf::Texture>>& frames) { impactFrames = frames; }


private:
    enum class ProjectileState { Flying, Impacting, Fading };

    sf::Sprite sprite;
    sf::Vector2f velocity;
    LeafAttackType type;
    ProjectileState state;

    // Animation
    std::vector<std::shared_ptr<sf::Texture>> frames;
    std::vector<std::shared_ptr<sf::Texture>> impactFrames;
    int currentFrame;
    float elapsedTime;
    float frameDuration;

    // State & Logic
    bool alive;
    bool damageDealt;
    bool hasTargetY;
    float targetY;
    float impactTimer;
    int impactFrame;

    // --- BIẾN MỚI CHO VÒNG ĐỜI ---
    float lifetime;    // Tổng thời gian đạn được phép tồn tại
    float lifeTimer;   // Bộ đếm thời gian đã trôi qua
};