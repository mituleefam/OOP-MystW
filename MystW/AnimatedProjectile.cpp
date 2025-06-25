#include "AnimatedProjectile.h"
AnimatedProjectile::AnimatedProjectile(const std::vector<std::shared_ptr<sf::Texture>>& frames,
    const sf::Vector2f& startPos,
    const sf::Vector2f& velocity,
    float frameDuration)
    : frames(frames), velocity(velocity), frameDuration(frameDuration) {
    sprite.setTexture(*frames[0]);
    sprite.setPosition(startPos);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    if (velocity.x < 0)
        sprite.setScale(-3.0f, 3.0f); // left direction
    else
        sprite.setScale(3.0f, 3.0f); // right direction
}

void AnimatedProjectile::update(float deltaTime) {
    elapsedTime += deltaTime;
    if (!frames.empty() && elapsedTime >= frameDuration) {
        elapsedTime = 0.0f;
        currentFrame = (currentFrame + 1) % frames.size();
        sprite.setTexture(*frames[currentFrame]);
        // Optionally setTextureRect if using a spritesheet
    }
    sprite.move(velocity * deltaTime);
}

void AnimatedProjectile::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::FloatRect AnimatedProjectile::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::FloatRect AnimatedProjectile::getHitBox() const {
    sf::FloatRect box = sprite.getGlobalBounds();
    // Shrink the hitbox by 80% to avoid hitting the player when the projectile is too close
    float shrink = 0.8f;
    box.left += box.width * shrink / 2.f;
    box.top += box.height * shrink / 2.f;
    box.width *= (1.f - shrink);
    box.height *= (1.f - shrink);
    return box;
}