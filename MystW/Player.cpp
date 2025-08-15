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

    // ĐỊNH NGHĨA HITBOX GỐC Ở ĐÂY (dựa trên kích thước frame 50x37)
    // Tinh chỉnh các giá trị này cho đến khi bạn hài lòng
    float hitboxWidth = 20.f;
    float hitboxHeight = 35.f;
    float hitboxLeft = (50.f - hitboxWidth) / 2.f; // Căn giữa theo chiều ngang
    float hitboxTop = (37.f - hitboxHeight);      // Đặt ở dưới cùng
    localHitbox = sf::FloatRect(hitboxLeft, hitboxTop, hitboxWidth, hitboxHeight);

    spirit.loadAssets();
}
void Player::Load()
{
	if (textureSheet.loadFromFile("Assets/Player/Textures/adventurer-v1.5-Sheet.png")) // Load texture
	{
		std::cout << "Player texture loaded successfully" << std::endl;
		sprite.setTexture(textureSheet); // Set the texture to the sprite
		sprite.setOrigin(25.0f, 37.0f); // Set the origin to the center of the sprite
        baseScale = 3.5f;
		//sprite.setScale(2.0f, 2.0f); // Scale the sprite to 5x its original size
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
    // --- BƯỚC 1: CẬP NHẬT CÁC BỘ ĐẾM THỜI GIAN ---
    // Luôn chạy đầu tiên, không phụ thuộc vào trạng thái của nhân vật.
    if (hurtCooldown > 0.0f) {
        hurtCooldown -= deltaTime;
        if (hurtCooldown < 0.0f) hurtCooldown = 0.0f;
    }
    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
        if (attackCooldown < 0.0f) attackCooldown = 0.0f;
    }

    // --- BƯỚC 2: XÁC ĐỊNH TRẠNG THÁI HIỆN TẠI (STATE DETERMINATION) ---
    // Đây là logic cốt lõi, hoạt động theo thứ tự ưu tiên để tránh xung đột.
    // Ưu tiên 1: Chết (Die)
    if (health <= 0) {
        animState = AnimationState::Die;
    }
    // Ưu tiên 2: Bị thương (Hurting) - **FIX QUAN TRỌNG NHẤT**
    else if (hurtCooldown > 0.0f) {
        animState = AnimationState::Hurting;
        isAttacking = false; // Nếu bị đánh, hủy đòn tấn công đang thực hiện.
    }
    // Ưu tiên 3: Đang tấn công (Attacking)
    else if (isAttacking) {
        // Nếu đang tấn công, giữ nguyên trạng thái. Logic animation ở dưới sẽ xử lý việc thoát khỏi trạng thái này.
    }
    // Ưu tiên 4: Các trạng thái di chuyển mặc định
    else {
        if (isJumping) {
            animState = AnimationState::Jumping;
        }
        else if (velocity.x != 0.0f) {
            animState = AnimationState::Running;
        }
        else {
            animState = AnimationState::Idle;
        }
    }


    // --- BƯỚC 4: XỬ LÝ INPUT TỪ NGƯỜI DÙNG ---
    // Chỉ nhận input nếu nhân vật không ở trong các trạng thái bận (chết, bị thương, đang tấn công).
    velocity.x = 0.0f;
    if (animState != AnimationState::Die && animState != AnimationState::Hurting && animState != AnimationState::Attacking) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            velocity.x = moveSpeed;
            isFacingRight = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            velocity.x = -moveSpeed;
            isFacingRight = false;
        }
    }

    // --- BƯỚC 5: VẬT LÝ VÀ VA CHẠM (PHYSICS & COLLISION) ---
    // Di chuyển ngang và xử lý va chạm tường
    sprite.move(velocity.x * deltaTime, 0);
    sf::FloatRect playerBounds = getHitBox();
    int tileSize = collisionLayer.getTileSize();
    for (float y = playerBounds.top; y < playerBounds.top + playerBounds.height; y += tileSize / 4.f) {
        if (velocity.x > 0 && collisionLayer.isCollidable(playerBounds.left + playerBounds.width, y)) {
            float offset = playerBounds.left - sprite.getPosition().x;
            float newX = (std::floor((playerBounds.left + playerBounds.width) / tileSize) * tileSize) - playerBounds.width - offset - 0.1f;
            sprite.setPosition(newX, sprite.getPosition().y);
            velocity.x = 0;
            break;
        }
        if (velocity.x < 0 && collisionLayer.isCollidable(playerBounds.left, y)) {
            float offset = playerBounds.left - sprite.getPosition().x;
            float newX = (std::floor(playerBounds.left / tileSize) * tileSize + tileSize) - offset + 0.1f;
            sprite.setPosition(newX, sprite.getPosition().y);
            velocity.x = 0;
            break;
        }
    }

    // Trọng lực và di chuyển dọc
    float currentGravity = (animState == AnimationState::AirAttacking) ? fastFallGravity : gravity;
    velocity.y += currentGravity * deltaTime;
    sprite.move(0, velocity.y * deltaTime);

    // Xử lý va chạm sàn và cập nhật trạng thái onGround/isJumping
    playerBounds = getHitBox();
    bool onGround = false;
    for (float x = playerBounds.left; x < playerBounds.left + playerBounds.width; x += tileSize / 4.f) {
        if (velocity.y >= 0 && collisionLayer.isCollidable(x, playerBounds.top + playerBounds.height)) {
            float offset = playerBounds.top - sprite.getPosition().y;
            // Sửa TILE_SIZE -> tileSize ở đây
            float newY = (std::floor((playerBounds.top + playerBounds.height) / tileSize) * tileSize) - playerBounds.height - offset;
            sprite.setPosition(sprite.getPosition().x, newY);
            velocity.y = 0;
            onGround = true;
            break;
        }
    }
    isJumping = !onGround;

    // Sau khi đã xử lý va chạm, nếu nhân vật đang chết và chạm đất thì dừng hẳn.
    if (animState == AnimationState::Die && onGround) {
        velocity.x = 0; // Đảm bảo không trượt ngang khi chết
    }

    // --- BƯỚC 6: XỬ LÝ HÀNH ĐỘNG (NHẢY, TẤN CÔNG) ---
    // Tương tự input, chỉ cho phép hành động khi không bận.
    if (animState != AnimationState::Die && animState != AnimationState::Hurting && animState != AnimationState::Attacking) {
        // Nhảy
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && onGround) {
            velocity.y = -jumpSpeed;
        }
        // Tấn công
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && attackCooldown <= 0.0f) {
            isAttacking = true;
            attackCooldown = attackCooldownDuration;
            animState = onGround ? AnimationState::Attacking : AnimationState::AirAttacking;
            // Reset animation khi bắt đầu hành động mới
            currentFrame = 0;
            animationTimer = 0.0f;

			attackRegistered = false; // Reset attack registered flag
        }
    }


    // --- BƯỚC 7: CẬP NHẬT ANIMATION ---
    // Khối code này quyết định frame nào sẽ được hiển thị.
    static AnimationState prevAnimState = AnimationState::Idle;
    if (animState != prevAnimState) {
        currentFrame = 0;
        animationTimer = 0.0f;
        prevAnimState = animState;
    }

    int frameCount = 1;
    float actualFrameDuration = frameDuration;
    switch (animState) {
    case AnimationState::Idle:          frameCount = 4; break;
    case AnimationState::Running:       frameCount = 6; break;
    case AnimationState::Jumping:       frameCount = 4; break;
    case AnimationState::AirAttacking:  frameCount = 7; actualFrameDuration = frameDuration * 0.3f; break;
    case AnimationState::Attacking:     frameCount = 11; actualFrameDuration = frameDuration * 0.3f; break;
    case AnimationState::Hurting:       frameCount = 3; actualFrameDuration = frameDuration * 0.4f; break;
    case AnimationState::Die:           frameCount = 6; actualFrameDuration = frameDuration * 1.5f; break;
    }

    animationTimer += deltaTime;
    if (animationTimer >= actualFrameDuration) {
        animationTimer = 0.0f;

        // Chỉ tăng frame nếu animation chưa kết thúc (đối với các anim không lặp lại)
        if (!((animState == AnimationState::Die || animState == AnimationState::Hurting) && currentFrame >= frameCount - 1)) {
            currentFrame++;
        }

        // Xử lý khi animation chạy hết một vòng
        if (currentFrame >= frameCount) {
            if (animState == AnimationState::Attacking || animState == AnimationState::AirAttacking) {
                isAttacking = false; // Đánh dấu đã tấn công xong, frame sau sẽ chuyển về trạng thái di chuyển.
            }
            // Với Hurting và Die, chúng ta không làm gì cả, cứ giữ ở frame cuối.
            // Các animation còn lại sẽ tự động lặp lại.
            currentFrame = 0;
        }
    }

    // --- BƯỚC 8: ÁP DỤNG TEXTURE VÀ HƯỚNG NHÌN CHO SPRITE ---
    switch (animState) {
    case AnimationState::Idle:          sprite.setTextureRect(idleFrames[currentFrame % 4]); break;
    case AnimationState::Running:       sprite.setTextureRect(runningFrames[currentFrame % 6]); break;
    case AnimationState::Jumping:       sprite.setTextureRect(jumpingFrames[currentFrame % 4]); break;
    case AnimationState::AirAttacking:  sprite.setTextureRect(airAttackingFrames[currentFrame % 7]); break;
    case AnimationState::Attacking:     sprite.setTextureRect(swordAttackingFrames[currentFrame % 11]); break;
    case AnimationState::Hurting:       sprite.setTextureRect(hurtingFrames[currentFrame % 3]); break;
    case AnimationState::Die:           sprite.setTextureRect(dieFrames[currentFrame >= 6 ? 5 : currentFrame]); break; // Giữ ở frame cuối
    default:                            sprite.setTextureRect(idleFrames[0]); break;
    }

    // Cập nhật hướng nhìn của sprite
    // Tinh chỉnh origin để sprite không bị "giật" khi quay người
    float originX = isFacingRight ? 20.0f : 30.0f;
    sprite.setOrigin(originX, 18.5f);
    sprite.setScale(isFacingRight ? baseScale : -baseScale, baseScale);
}

void Player::takeDamage(int damage)
{
	if (hurtCooldown <= 0.0f) {
		health -= damage;
        // isAttacked = true; // This is now redundant since we check hurtCooldown directly. We can remove it.
		//animState = AnimationState::Hurting;
		hurtCooldown = hurtCooldownDuration;

		//velocity.x = isFacingRight ? -knockbackForce : knockbackForce;

		if (health <= 0) {
			health = 0;
			std::cout << "Player has died!" << std::endl;
            // The Update loop will now automatically handle setting the Die state
			//animState = AnimationState::Die;
			//isAttacking = false;
		}

		std::cout << "Player took " << damage << " damage!" << std::endl;
        isAttacked = true;
	}
	else {
		std::cout << "Player is still recovering (hurtCooldown active)!" << std::endl;
	}
}
sf::FloatRect Player::getHitBox() const {
    return sprite.getTransform().transformRect(localHitbox);
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

    // === THÊM ĐOẠN NÀY ĐỂ VẼ HITBOX TẤN CÔNG ===
    // Chỉ vẽ khi đang ở trạng thái tấn công
    if (animState == AnimationState::Attacking || animState == AnimationState::AirAttacking) {
        sf::RectangleShape attackBox;
        sf::FloatRect attackHitbox = getAttackBounds();
        attackBox.setPosition(attackHitbox.left, attackHitbox.top);
        attackBox.setSize(sf::Vector2f(attackHitbox.width, attackHitbox.height));
        attackBox.setFillColor(sf::Color(0, 255, 0, 150)); // Màu xanh cho tấn công
        window.draw(attackBox);
    }
}
sf::FloatRect Player::getAttackBounds() const {
	float width = 50.f;
	float height = 50.f;
	sf::Vector2f pos = sprite.getPosition();
    // OffsetX: Khoảng cách từ tâm nhân vật tới điểm bắt đầu của hitbox tấn công.
    // Nếu isFacingRight, hitbox bắt đầu ở phía trước.
    // Nếu không, nó bắt đầu ở phía trước (bên trái), nhưng cần trừ đi cả chiều rộng của hitbox.
	float offsetX = isFacingRight ? 20.f : -20.f - width;
	return sf::FloatRect(pos.x + offsetX, pos.y - (height / 2.f), width, height);
}

bool Player::isDead() const {
    return health <= 0;
}