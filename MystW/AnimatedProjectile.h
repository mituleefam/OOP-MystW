#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Animation.h"

class AnimatedProjectile {
public:
    AnimatedProjectile(const std::vector<std::shared_ptr<sf::Texture>>& frames,
        const sf::Vector2f& startPos,
        const sf::Vector2f& velocity,
        float frameDuration = 0.1f);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
	bool isExpired() const;
    float lifetime;
    sf::FloatRect getHitBox() const;
private:
    sf::Sprite sprite;
    std::vector<std::shared_ptr<sf::Texture>> frames;
    sf::Vector2f velocity;
    float frameDuration;
    float elapsedTime = 0.f;
    int currentFrame = 0;
    sf::Clock lifeTimer;
};

