#pragma once
#include <SFML/Graphics.hpp>
class SpellEffect
{
public:
    SpellEffect(const sf::Texture& texture, const sf::Vector2f& pos, sf::Vector2i frameSize, int frameCount, float duration = 0.25f)
        : sprite(texture), position(pos), frameSize(frameSize), frameCount(frameCount), duration(duration), timer(0.f), active(true), currentFrame(0)
    {
        sprite.setPosition(position);
        sprite.setOrigin(frameSize.x / 2.f, 0.f); // Center X, top Y
        sprite.setScale(3.0f, 3.0f); // Resize accordingly
        sprite.setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
        frameDuration = duration / std::max(1, frameCount);
    }

    void update(float dt) {
        if (!active) return;
        timer += dt;
        if (timer >= duration) {
            active = false;
            return;
        }
        // Animation logic
        int newFrame = static_cast<int>(timer / frameDuration);
        if (newFrame != currentFrame && newFrame < frameCount) {
            currentFrame = newFrame;
            sprite.setTextureRect(sf::IntRect(frameSize.x * currentFrame, 0, frameSize.x, frameSize.y));
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (active) window.draw(sprite);
    }

    bool isActive() const { return active; }
    sf::FloatRect getHitBox() const { return sprite.getGlobalBounds(); }
    // --- ADDED: Methods to track damage ---
    bool hasDealtDamage() const { return damageDealt; }
    void setDamageDealt(bool dealt) { damageDealt = dealt; }

private:
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2i frameSize;
    int frameCount;
    float duration;
    float timer;
    float frameDuration;
    int currentFrame;
    bool active;
    bool damageDealt; // Flag to ensure one-time damage
};
