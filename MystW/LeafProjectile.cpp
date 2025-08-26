#include "LeafProjectile.h"
#include <iostream>

LeafProjectile::LeafProjectile(
    const std::vector<std::shared_ptr<sf::Texture>>& frames,
    const sf::Vector2f& spawnPos,
    const sf::Vector2f& velocity,
    float frameDuration,
    LeafAttackType type,
    float lifetime)
    : frames(frames), velocity(velocity), frameDuration(frameDuration),
    type(type), state(ProjectileState::Flying),
    currentFrame(0), elapsedTime(0.f),
    alive(true), damageDealt(false), hasTargetY(false), targetY(0.f),
    impactFrame(0), impactTimer(0.f),
    lifetime(lifetime), lifeTimer(0.f)
{
    if (!frames.empty()) {
        sprite.setTexture(*frames[0]);
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

        // Specific scaling for different projectile types
        switch (type) {
        case LeafAttackType::PoisonArrow:
        case LeafAttackType::EntangleArrow:
            sprite.setScale(3.f, 3.f);
            break;
        case LeafAttackType::Rain:
            sprite.setScale(3.0f, 3.0f);
            break;
        case LeafAttackType::Impact:
            sprite.setScale(3.5f, 3.5f);
            break;
        case LeafAttackType::Downward:
            sprite.setScale(2.5f, 2.5f);
            break;
        case LeafAttackType::Beam:
            sprite.setScale(2.2f, 2.2f);
            break;
        default:
            sprite.setScale(3.5f, 3.5f);
            break;
        }
    }

    sprite.setPosition(spawnPos);

    // Flip sprite based on velocity
    if (velocity.x < 0) {
        sprite.setScale(-std::abs(sprite.getScale().x), sprite.getScale().y);
    }
}

void LeafProjectile::update(float deltaTime) {
    if (!alive) return;

    // Đếm thời gian và tự hủy nếu hết giờ.
    lifeTimer += deltaTime;
    if (lifeTimer >= lifetime) {
        alive = false;
        return; // Đạn chết, không cần xử lý gì thêm
    }

    elapsedTime += deltaTime;

    // Handle state-specific logic
    switch (state) {
    case ProjectileState::Flying:
        sprite.move(velocity * deltaTime);

        // Check if the projectile reached its target height (for rain attack)
        if (hasTargetY && sprite.getPosition().y >= targetY) {
            if (!impactFrames.empty()) {
                state = ProjectileState::Impacting;
                sprite.setTexture(*impactFrames[0]);
                sprite.setPosition(sprite.getPosition().x, targetY);
                velocity = { 0.f, 0.f }; // Stop moving
                currentFrame = 0;
                elapsedTime = 0.f;
            }
            else {
                alive = false; // No impact animation, just disappear
            }
        }
        break;

    case ProjectileState::Impacting:
        // The impact animation is playing. It will be handled by the animation logic below.
        // The projectile doesn't move in this state.
        break;

    case ProjectileState::Fading:
        // Logic for fading out if needed, for now, it just gets removed.
        break;
    }

    // Handle animation
    if (elapsedTime >= frameDuration) {
        elapsedTime -= frameDuration;
        currentFrame++;

        const auto& activeFrames = (state == ProjectileState::Impacting) ? impactFrames : frames;

        if (currentFrame >= activeFrames.size()) {
            // If the animation is over
            if (state == ProjectileState::Impacting || type == LeafAttackType::Beam) {
                alive = false; // Projectile dies after impact/beam animation
            }
            currentFrame = 0; // Loop for flying projectiles
        }

        if (alive && !activeFrames.empty()) {
            sprite.setTexture(*activeFrames[currentFrame]);
        }
    }

}

void LeafProjectile::draw(sf::RenderWindow& window) const {
    if (alive) {
        window.draw(sprite);
    }
}

sf::FloatRect LeafProjectile::getHitBox() const {
    if (!alive) return {};

    sf::FloatRect box = sprite.getGlobalBounds();
    // Shrink the hitbox to be more accurate
    float shrinkX = 0.5f;
    float shrinkY = 0.5f;
    box.left += box.width * shrinkX / 2.0f;
    box.top += box.height * shrinkY / 2.0f;
    box.width *= (1.0f - shrinkX);
    box.height *= (1.0f - shrinkY);
    return box;
}