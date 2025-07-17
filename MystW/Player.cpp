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
		sprite.setPosition(400.0f, 900.0f); // Set the initial position of the player sprite
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
		}
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

	// --- INPUT HANDLING ---
	//bool onGround = false; // We will determine this with collision checks later
	velocity.x = 0.0f; // Reset horizontal velocity each frame

	// Only allow movement if not in a state that locks movement (like Hurting)
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

	// --- PHYSICS & COLLISION (The new core logic) ---

	// 1. HORIZONTAL MOVEMENT & COLLISION
	sprite.move(velocity.x * deltaTime, 0);
	sf::FloatRect playerBounds = getHitBox();
	int tileSize = collisionLayer.getTileSize();

	for (int y = playerBounds.top; y < playerBounds.top + playerBounds.height; y += tileSize / 4) {
		if (velocity.x > 0) { // Moving right
			if (collisionLayer.isCollidable(playerBounds.left + playerBounds.width, y)) {
				sprite.setPosition((int)(playerBounds.left + playerBounds.width) / tileSize * tileSize - playerBounds.width, sprite.getPosition().y);
				velocity.x = 0;
				break;
			}
		}
		else if (velocity.x < 0) { // Moving left
			if (collisionLayer.isCollidable(playerBounds.left, y)) {
				sprite.setPosition((int)playerBounds.left / tileSize * tileSize + tileSize, sprite.getPosition().y);
				velocity.x = 0;
				break;
			}
		}
	}

	// 2. VERTICAL MOVEMENT & COLLISION (GRAVITY)
	float currentGravity = (animState == AnimationState::AirAttacking) ? fastFallGravity : gravity;
	velocity.y += currentGravity * deltaTime;

	bool onGround = false; // Reset onGround each frame
	sprite.move(0, velocity.y * deltaTime);
	playerBounds = getHitBox(); // Get updated bounds after moving

	for (int x = playerBounds.left; x < playerBounds.left + playerBounds.width; x += tileSize / 4) {
		if (velocity.y > 0) { // Moving down
			if (collisionLayer.isCollidable(x, playerBounds.top + playerBounds.height)) {
				sprite.setPosition(sprite.getPosition().x, (int)(playerBounds.top + playerBounds.height) / tileSize * tileSize - playerBounds.height);
				velocity.y = 0;
				onGround = true; // Landed on the ground (only here we set onGround true)
				isJumping = false;
				break;
			}
		}
		else if (velocity.y < 0) { // Moving up
			if (collisionLayer.isCollidable(x, playerBounds.top)) {
				sprite.setPosition(sprite.getPosition().x, (int)playerBounds.top / tileSize * tileSize + tileSize);
				velocity.y = 0; // Bonked head on ceiling
				break;
			}
		}
	}

	// A final check to see if we are standing on something
	// This helps if the player walks off a ledge
	if (!onGround) {
		//isJumping = true;
		bool isSupported = false;
		for (int x = playerBounds.left; x < playerBounds.left + playerBounds.width; x += tileSize / 4) {
			if (collisionLayer.isCollidable(x, playerBounds.top + playerBounds.height + 1)) {
				isSupported = true;
				break;
			}
		}
		onGround = isSupported;
		if (!onGround) {
			isJumping = true; // If not on ground, we must be in the air
		}
	}


	// --- JUMP & ATTACK INPUT (Based on new `onGround` state) ---
	if (animState != AnimationState::Hurting) {
		// Use Space to jump, a common convention. You can change it back to 'W'.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && onGround) {
			velocity.y = -jumpSpeed;
			isJumping = true;
			onGround = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && attackCooldown <= 0.0f) {
			isAttacking = true;
			//currentFrame = 0;
			//animationTimer = 0.0f;
			attackCooldown = attackCooldownDuration;
		}
	}


	// --- ANIMATION STATE LOGIC (Old logic, slightly adapted) ---
	if (animState != AnimationState::Hurting) { // Do not change state if hurting
		if (isAttacking) {
			animState = onGround ? AnimationState::Attacking : AnimationState::AirAttacking;
		}
		else if (isJumping) {
			animState = AnimationState::Jumping;
		}
		else if (velocity.x != 0) {
			animState = AnimationState::Running;
		}
		else {
			animState = AnimationState::Idle;
		}
	}

	// ------ ANIMMATION ------
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
	case AnimationState::Hurting: frameCount = 3; actualFrameDuration = frameDuration * 0.4f; break;
	}

	animationTimer += deltaTime;
	if (animationTimer >= actualFrameDuration) {
		animationTimer = 0.0f;
		currentFrame++;
		if (currentFrame >= frameCount) {
			currentFrame = 0;
			if (animState == AnimationState::Attacking || animState == AnimationState::AirAttacking) {
				isAttacking = false;
				//attackRegistered = false;
				//animState = isJumping ? AnimationState::Jumping : AnimationState::Idle;
			}
			//if (animState == AnimationState::Attacking) {
			//	isAttacking = false;
			//	attackRegistered = false; // Reset for next attack
			//	animState = AnimationState::Idle;
			//}
			if (animState == AnimationState::Hurting) {
				isAttacked = false;
				//animState = AnimationState::Idle;
			}
		}
	}

	// Set sprite texture rect (protect against out-of-bounds)
	switch (animState) {
	case AnimationState::Idle:			sprite.setTextureRect(idleFrames[currentFrame % 4]); break;
	case AnimationState::Running:		sprite.setTextureRect(runningFrames[currentFrame % 6]); break;
	case AnimationState::Jumping:		sprite.setTextureRect(jumpingFrames[currentFrame % 4]); break;
	case AnimationState::AirAttacking:	sprite.setTextureRect(airAttackingFrames[currentFrame % 7]);	break;
	case AnimationState::Attacking:		sprite.setTextureRect(swordAttackingFrames[currentFrame % 11]); break;
	case AnimationState::Hurting:		sprite.setTextureRect(hurtingFrames[currentFrame % 3]); break;
	default:							sprite.setTextureRect(idleFrames[0]); break;
	}

	// ------ SET DIRECTION ------
	// Set scale and origin based on facing direction
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
	float shrink = 0.7f; // Shrink hitbox by 70% ()
	box.left += box.width * shrink / 2.0f;
	box.top += box.height * shrink / 2.0f;
	box.width *= (1.0f - shrink);
	box.height *= (1.0f - shrink);
	return box;
}
void Player::Draw(sf::RenderWindow& window)
{
	window.draw(sprite);
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