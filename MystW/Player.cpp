#include "Player.h"
#include <iostream>

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
		sprite.setPosition(150.0f, 700.0f); // Set the initial position of the player sprite
	}
	else
	{
		std::cerr << "Error loading player texture" << std::endl;
		return; // Exit if the texture fails to load
	}
}

void Player::Update(float deltaTime)
{
	if (hurtCooldown > 0.0f) {
		hurtCooldown -= deltaTime;
		if (hurtCooldown < 0.0f) hurtCooldown = 0.0f;
	}
	if (attackCooldown > 0.0f) {
		attackCooldown -= deltaTime;
		if (attackCooldown < 0.0f) attackCooldown = 0.0f; // Prevent negative cooldown
	}

	// Track previous state to reset frame only on state change
	static AnimationState prevAnimState = animState;

	// Die animation handling
	if (animState == AnimationState::Die) {
		//int dieFrameCount = 6;
		int dieFrameCount = sizeof(dieFrames) / sizeof(dieFrames[0]); // More robust frame count
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
				// Reset to the last frame if animation is still playing
				sprite.setTextureRect(dieFrames[dieFrameCount - 1]);
			}
			//else {
			//	// No frames, hide the sprite or set a flag
			//	sprite.setColor(sf::Color(255, 255, 255, 0)); // Hide sprite
			//}
		}
		//else {
		//	// Animation finished, hide the sprite or set a flag
		//	sprite.setColor(sf::Color(255, 255, 255, 0));
		//	// Or use a flag for other logic
		//}

		velocity.x = 0.f; // Stop horizontal movement

		return;
	}

	// 
	if (isAttacked && animState != AnimationState::Hurting) {
		isAttacking = false;
		animState = AnimationState::Hurting;
		currentFrame = 0;
		animationTimer = 0.0f;
		hurtCooldown = hurtCooldownDuration;
		std::cout << "Player is attacked!" << std::endl;
	}

	// Handle input and state transitions

	if (animState != AnimationState::Hurting && !isAttacking) {
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) && !isJumping) {
			animState = AnimationState::Running;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !isJumping) {
			animState = AnimationState::Jumping;
			velocity.y = -jumpSpeed;
			isJumping = true;
		}
		else if (!isJumping) {
			animState = AnimationState::Idle;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && attackCooldown <= 0.0f) {
			if (isJumping) {
				animState = AnimationState::AirAttacking;
			}
			else {
				animState = AnimationState::Attacking;
			}
			isAttacking = true;
			attackRegistered = false;
			attackCooldown = attackCooldownDuration;

			currentFrame = 0;         // <<-- ADDED: Ensure attack animation restarts
			animationTimer = 0.0f;    // <<-- ADDED: Ensure attack animation timer restarts
		}
	}

	// Reset frame if state changed
	if (animState != prevAnimState) {
		currentFrame = 0;
		prevAnimState = animState;
	}

	// Animation frame update
	int frameCount = 1;
	float actualFrameDuration = frameDuration;
	switch (animState) {
	case AnimationState::Idle: frameCount = 4; break;
	case AnimationState::Running: frameCount = 6; break;
	case AnimationState::Jumping: frameCount = 4; break;
	case AnimationState::AirAttacking: frameCount = 7; actualFrameDuration = frameDuration * 0.3f; break;
	case AnimationState::Attacking: frameCount = 11; actualFrameDuration = frameDuration * 0.3f; break;
	case AnimationState::Hurting: frameCount = 3; actualFrameDuration = frameDuration * 0.5f; break;
	}

	animationTimer += deltaTime;
	if (animationTimer >= actualFrameDuration) {
		animationTimer = 0.0f;
		currentFrame++;
		if (currentFrame >= frameCount) {
			currentFrame = 0;
			if (animState == AnimationState::AirAttacking) {
				isAttacking = false;
				attackRegistered = false;
				animState = isJumping ? AnimationState::Jumping : AnimationState::Idle;
			}
			if (animState == AnimationState::Attacking) {
				isAttacking = false;
				attackRegistered = false; // Reset for next attack
				animState = AnimationState::Idle;
			}
			if (animState == AnimationState::Hurting) {
				isAttacked = false;
				animState = AnimationState::Idle;
			}
		}
	}

	// Set sprite texture rect (protect against out-of-bounds)
	switch (animState) {
	case AnimationState::Idle:
		sprite.setTextureRect(idleFrames[currentFrame % 4]);
		break;
	case AnimationState::Running:
		sprite.setTextureRect(runningFrames[currentFrame % 6]);
		break;
	case AnimationState::Jumping:
		sprite.setTextureRect(jumpingFrames[currentFrame % 4]);
		break;
	case AnimationState::AirAttacking:
		sprite.setTextureRect(airAttackingFrames[currentFrame % 7]);
		break;
	case AnimationState::Attacking:
		sprite.setTextureRect(swordAttackingFrames[currentFrame % 11]);
		break;
	case AnimationState::Hurting:
		sprite.setTextureRect(hurtingFrames[currentFrame % 3]);
		break;
	default:
		sprite.setTextureRect(idleFrames[0]);
		break;
	}

	// Prevent movement while attacking or hurting
	if (!isAttacking && animState != AnimationState::Hurting) {
		velocity.x = 0.0f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			velocity.x += moveSpeed;
			isFacingRight = true;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			velocity.x -= moveSpeed;
			isFacingRight = false;
		}
	}
	else {
		velocity.x = 0.0f;
	} // if use knockBackForce: comment this and uncomment in takeDamage

	// Set scale and origin based on facing direction
	float originX = isFacingRight ? 20.0f : 25.0f;
	sprite.setOrigin(originX, 18.5f);
	sprite.setScale(isFacingRight ? 5.0f : -5.0f, 5.0f);

	// Gravity
	velocity.y += gravity;
	sprite.move(velocity);

	// Check enemy collision
	//sf::FloatRect playerBounds = getHitBox();
	//sf::FloatRect enemyBounds = enemy.getHitBox();
	//if (playerBounds.intersects(enemyBounds))
	//{
	//	// Push the player away from the enemy
	//	if (sprite.getPosition().x < enemy.getPosition().x)
	//		sprite.setPosition(enemyBounds.left - playerBounds.width / 2.f, sprite.getPosition().y);
	//	else
	//		sprite.setPosition(enemyBounds.left + enemyBounds.width + playerBounds.width / 2.f, sprite.getPosition().y);
	//	velocity.x = 0.0f; // Stop horizontal movement on collision
	//	return; // Exit early to prevent further processing
	//}
	//float px = sprite.getPosition().x;
	//float py = sprite.getPosition().y;
	//float minX = 0 + playerBounds.width / 2.f;
	//float maxX = 1920 - playerBounds.width / 2.f; // 1920 is window width
	//if (px < minX) px = minX;
	//if (px > maxX) px = maxX;
	//sprite.setPosition(px, py);


	// Ground collision
	if (sprite.getPosition().y >= groundY) {
		sprite.setPosition(sprite.getPosition().x, groundY);
		velocity.y = 0.0f;
		isJumping = false;
	}
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
	float shrink = 0.7f; // Shrink hitbox by 70% ()
	box.left += box.width * shrink / 2.0f;
	box.top += box.height * shrink / 2.0f;
	box.width *= (1.0f - shrink);
	box.height *= (1.0f - shrink);
	return box;
}
void Player::Draw()
{

}