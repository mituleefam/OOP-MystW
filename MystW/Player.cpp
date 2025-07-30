#include "Player.h"
#include <iostream>
#include "CollisionLayer.hpp" // Include to use collision logic

void Player::Initialize()
{
	// Initialize animation frames
	idleFrames[0] = sf::IntRect(0, 0, 50, 37);
	idleFrames[1] = sf::IntRect(50, 0, 50, 37);
	idleFrames[2] = sf::IntRect(100, 0, 50, 37);
	idleFrames[3] = sf::IntRect(150, 0, 50, 37);

	runningFrames[0] = sf::IntRect(0, 37, 50, 37);
	runningFrames[1] = sf::IntRect(50, 37, 50, 37);
	runningFrames[2] = sf::IntRect(100, 37, 50, 37);
	runningFrames[3] = sf::IntRect(150, 37, 50, 37);
	runningFrames[4] = sf::IntRect(200, 37, 50, 37);
	runningFrames[5] = sf::IntRect(250, 37, 50, 37);

	jumpingFrames[0] = sf::IntRect(0, 74, 50, 37);
	jumpingFrames[1] = sf::IntRect(50, 74, 50, 37);
	jumpingFrames[2] = sf::IntRect(100, 74, 50, 37);
	jumpingFrames[3] = sf::IntRect(150, 74, 50, 37);

	swordAttackingFrames[0] = sf::IntRect(0, 222, 50, 37);
	swordAttackingFrames[1] = sf::IntRect(50, 222, 50, 37);
	swordAttackingFrames[2] = sf::IntRect(100, 222, 50, 37);
	swordAttackingFrames[3] = sf::IntRect(150, 222, 50, 37);
	swordAttackingFrames[4] = sf::IntRect(200, 222, 50, 37);
	swordAttackingFrames[5] = sf::IntRect(250, 222, 50, 37);
	swordAttackingFrames[6] = sf::IntRect(300, 222, 50, 37);
	swordAttackingFrames[7] = sf::IntRect(0, 259, 50, 37);
	swordAttackingFrames[8] = sf::IntRect(50, 259, 50, 37);
	swordAttackingFrames[9] = sf::IntRect(100, 259, 50, 37);
	swordAttackingFrames[10] = sf::IntRect(150, 259, 50, 37);

	airAttackingFrames[0] = sf::IntRect(200, 518, 50, 37);
	airAttackingFrames[1] = sf::IntRect(250, 518, 50, 37);
	airAttackingFrames[2] = sf::IntRect(300, 518, 50, 37);
	airAttackingFrames[3] = sf::IntRect(0, 555, 50, 37);
	airAttackingFrames[4] = sf::IntRect(50, 555, 50, 37);
	airAttackingFrames[5] = sf::IntRect(100, 555, 50, 37);
	airAttackingFrames[6] = sf::IntRect(150, 555, 50, 37);

	hurtingFrames[0] = sf::IntRect(0, 333, 50, 37);
	hurtingFrames[1] = sf::IntRect(50, 333, 50, 37);
	hurtingFrames[2] = sf::IntRect(100, 333, 50, 37);

	dieFrames[0] = sf::IntRect(0, 333, 50, 37);
	dieFrames[1] = sf::IntRect(50, 333, 50, 37);
	dieFrames[2] = sf::IntRect(100, 333, 50, 37);
	dieFrames[3] = sf::IntRect(150, 333, 50, 37);
	dieFrames[4] = sf::IntRect(200, 333, 50, 37);
	dieFrames[5] = sf::IntRect(250, 333, 50, 37);
}
void Player::Load()
{
	if (textureSheet.loadFromFile("Assets/Player/Textures/adventurer-v1.5-Sheet.png")) // Load texture
	{
		std::cout << "Player texture loaded successfully" << std::endl;
		sprite.setTexture(textureSheet); // Set the texture to the sprite
		sprite.setOrigin(25.0f, 18.5f); // Set the origin to the center of the sprite
		sprite.setScale(5.0f, 5.0f); // Scale the sprite to 5x its original size
		sprite.setPosition(400.0f, 1040.0f); // Set the initial position of the player sprite
	}
	else
	{
		std::cerr << "Error loading player texture" << std::endl;
		return; // Exit if the texture fails to load
	}
}

void Player::Update(float deltaTime, CollisionLayer& collisionLayer)
{
    if (hurtCooldown > 0.0f) {
        hurtCooldown -= deltaTime;
        if (hurtCooldown < 0.0f) hurtCooldown = 0.0f;
    }
    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
        if (attackCooldown < 0.0f) attackCooldown = 0.0f;
    }

    static AnimationState prevAnimState = animState;

    if (animState == AnimationState::Die) {
        int dieFrameCount = sizeof(dieFrames) / sizeof(dieFrames[0]);
        float dieFrameDuration = frameDuration * 1.5f;
        animationTimer += deltaTime;

        if (currentFrame < dieFrameCount) {
            if (animationTimer >= dieFrameDuration) {
                animationTimer = 0.0f;
                currentFrame++;
            }
            if (currentFrame < dieFrameCount)
                sprite.setTextureRect(dieFrames[currentFrame]);
            else if (dieFrameCount > 0) {
                sprite.setTextureRect(dieFrames[dieFrameCount - 1]);
            }
        }
        velocity.x = 0.f;
        return;
    }

    if (isAttacked && animState != AnimationState::Hurting) {
        isAttacking = false;
        animState = AnimationState::Hurting;
        currentFrame = 0;
        animationTimer = 0.0f;
        hurtCooldown = hurtCooldownDuration;
        std::cout << "Player is attacked!" << std::endl;
    }

    velocity.x = 0.0f;

    if (animState != AnimationState::Hurting && !isAttacking) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            velocity.x += moveSpeed;
            isFacingRight = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            velocity.x -= moveSpeed;
            isFacingRight = false;
        }
    }

    int tileSize = collisionLayer.getTileSize();
    sf::FloatRect playerBounds = getHitBox();

    sprite.move(velocity.x * deltaTime, 0);
    playerBounds = getHitBox();
    sf::FloatRect hb = getHitBox();
    if (hb.left <= 0.f + tileSize / 2.f && velocity.x == 0.f && velocity.y == 0.f) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

            return;
        }
    }



    for (int y = playerBounds.top; y < playerBounds.top + playerBounds.height; y += tileSize / 4) {
        if (velocity.x > 0 && collisionLayer.isCollidable(playerBounds.left + playerBounds.width, y)) {
            float snapX = ((int)((playerBounds.left + playerBounds.width) / tileSize)) * tileSize - playerBounds.width;
            sprite.setPosition(snapX, sprite.getPosition().y);
            velocity.x = 0.f;
            break;
        }
        else if (velocity.x < 0 && collisionLayer.isCollidable(playerBounds.left, y)) {
            float snapX = ((int)(playerBounds.left / tileSize) + 1) * tileSize;
            sprite.setPosition(snapX, sprite.getPosition().y);
            velocity.x = 0.f;
            break;
        }
    }

    float currentGravity = (animState == AnimationState::AirAttacking) ? fastFallGravity : gravity;
    velocity.y += currentGravity * deltaTime;

    sprite.move(0, velocity.y * deltaTime);
    playerBounds = getHitBox();

    bool onGround = false;

    for (int x = playerBounds.left; x < playerBounds.left + playerBounds.width; x += tileSize / 4) {
        if (x >= 0 && velocity.y > 0 && collisionLayer.isCollidable(x, playerBounds.top + playerBounds.height)) {
            // Snap to ground
            float offset = getHitBox().top - sprite.getPosition().y;
            float newY = std::floor((getHitBox().top + getHitBox().height) / tileSize) * tileSize - getHitBox().height - offset;
            sprite.setPosition(sprite.getPosition().x, newY);
            velocity.y = 0;
            onGround = true;
            isJumping = false;
            break;
        }
    }


    if (!onGround) {
        for (int x = playerBounds.left; x < playerBounds.left + playerBounds.width; x += tileSize / 4) {
            if (x >= 0 && collisionLayer.isCollidable(x, playerBounds.top + playerBounds.height + 1)) {
                onGround = true;
                break;
            }
        }
        isJumping = !onGround;
    }


    if (animState != AnimationState::Hurting) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && onGround) {
            velocity.y = -jumpSpeed;
            isJumping = true;
            onGround = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && !isAttacking && attackCooldown <= 0.0f) {
            isAttacking = true;
            attackCooldown = attackCooldownDuration;
            animState = onGround ? AnimationState::Attacking : AnimationState::AirAttacking;
            currentFrame = 0;
            animationTimer = 0.0f;
        }
    }

    if (!isAttacking && animState != AnimationState::Hurting) {
        if (isJumping)
            animState = AnimationState::Jumping;
        else if (velocity.x != 0)
            animState = AnimationState::Running;
        else
            animState = AnimationState::Idle;
    }

    if (animState != prevAnimState) {
        currentFrame = 0;
        prevAnimState = animState;
    }

    int frameCount = 1;
    float actualFrameDuration = frameDuration;
    switch (animState) {
    case AnimationState::Idle: frameCount = 4; break;
    case AnimationState::Running: frameCount = 6; break;
    case AnimationState::Jumping: frameCount = 4; break;
    case AnimationState::AirAttacking: frameCount = 7; actualFrameDuration = frameDuration * 0.3f; break;
    case AnimationState::Attacking: frameCount = 11; actualFrameDuration = frameDuration * 0.3f; break;
    case AnimationState::Hurting: frameCount = 3; actualFrameDuration = frameDuration * 0.4f; break;
    }

    animationTimer += deltaTime;
    if (animationTimer >= actualFrameDuration) {
        animationTimer = 0.0f;
        currentFrame++;

        std::cout << "State: " << static_cast<int>(animState) << ", Frame: " << currentFrame << ", isAttacking: " << isAttacking << "\n";

        if (currentFrame >= frameCount) {
            currentFrame = 0;
            if (animState == AnimationState::Attacking || animState == AnimationState::AirAttacking) {
                isAttacking = false;
            }
            if (animState == AnimationState::Hurting) {
                isAttacked = false;
            }
        }
    }

    switch (animState) {
    case AnimationState::Idle: sprite.setTextureRect(idleFrames[currentFrame % 4]); break;
    case AnimationState::Running: sprite.setTextureRect(runningFrames[currentFrame % 6]); break;
    case AnimationState::Jumping: sprite.setTextureRect(jumpingFrames[currentFrame % 4]); break;
    case AnimationState::AirAttacking: sprite.setTextureRect(airAttackingFrames[currentFrame % 7]); break;
    case AnimationState::Attacking: sprite.setTextureRect(swordAttackingFrames[currentFrame % 11]); break;
    case AnimationState::Hurting: sprite.setTextureRect(hurtingFrames[currentFrame % 3]); break;
    default: sprite.setTextureRect(idleFrames[0]); break;
    }

    float originX = isFacingRight ? 20.0f : 30.0f;
    sprite.setOrigin(originX, 18.5f);
    sprite.setScale(isFacingRight ? 5.0f : -5.0f, 5.0f);
}

void Player::takeDamage(int damage)
{
	if (hurtCooldown <= 0.0f) {
		health -= damage;
		isAttacked = true;
		//animState = AnimationState::Hurting;
		hurtCooldown = hurtCooldownDuration;

		//velocity.x = isFacingRight ? -knockbackForce : knockbackForce;

		if (health <= 0) {
			health = 0;
			std::cout << "Player has died!" << std::endl;
			animState = AnimationState::Die;
			isAttacking = false;
		}

		std::cout << "Player took " << damage << " damage!" << std::endl;
	}
	else {
		std::cout << "Player is still recovering (hurtCooldown active)!" << std::endl;
	}
}
sf::FloatRect Player::getHitBox() const {
	if (health <= 0) {
		// Player is dead, return an empty hitbox
		return sf::FloatRect(0, 0, 0, 0);
	} // to stop the enemy from hitting the player when dead

	sf::FloatRect box = sprite.getGlobalBounds();
    float shrinkX = 0.2f; // 20% chiều ngang
    float shrinkY = 0.1f; // 10% chiều dọc
    box.left += box.width * shrinkX / 2.0f;
    box.top += box.height * shrinkY / 2.0f;
    box.width *= (1.0f - shrinkX);
    box.height *= (1.0f - shrinkY);
    return box;
}
void Player::Draw(sf::RenderWindow& window)
{
	window.draw(sprite);
	sf::FloatRect hitbox = getHitBox();
	sf::RectangleShape box;
	box.setPosition(hitbox.left, hitbox.top);
	box.setSize(sf::Vector2f(hitbox.width, hitbox.height));
	box.setFillColor(sf::Color(255, 0, 0, 100));
	window.draw(box);

}
sf::FloatRect Player::getAttackBounds() const {
	//     if (animState != AnimationState::Attacking)
			 //return sf::FloatRect(); // No attack bounds if not attacking;
			 // No need cause in main loop we check if the player is attacking before calling this function
	float width = 10.f;
	float height = 20.f;
	sf::Vector2f pos = sprite.getPosition();
	float offsetX = isFacingRight ? 15.f : -45.f; // 
	return sf::FloatRect(pos.x + offsetX, pos.y - height, width, height);
}