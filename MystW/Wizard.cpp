#include "Wizard.h"
#include "Player.h"
#include "Math.h" // For normalizeVector, assuming it's in Math.h
#include <iostream> // For debugging

// Define the static member for Wizard animation info
const std::map<EnemyState, AnimSheetInfo> Wizard::wizardAnimSheetInfos = {
    { EnemyState::Idle,        { "Idle.png",   231, 190, 6, 0.15f, true  }},
    { EnemyState::Running,     { "Run.png",    231, 190, 8, 0.15f,  true  }},
    { EnemyState::Attacking,   { "Attack.png", 231, 190, 8, 0.1f, false }}, // Non-looping attack, casting a spell
    { EnemyState::Attacking2,  { "Attack2.png",231, 190, 8, 0.15f, false }}, // melee attack
    { EnemyState::Hurt,        { "Hurt.png",   231, 190, 4, 0.15f, false }},
    { EnemyState::Dying,       { "Die.png",    231, 190, 7, 0.5f,  false }}
};

Wizard::Wizard(const std::string& assetBaseFolder, float startX, float startY)
    : Enemy(startX, startY, 7, 125.0f), // Base constructor: x, y, max health, speed
    baseAssetPath(assetBaseFolder),
    spellInterval(3.0f), // Wizard attacks every 3 second
    spellRange(700.0f),  // Wizard attacks if player is within 200px
    meleeInterval(0.2f), // Melee attack every 0.2 second
    meleeRange(300.0f),
    detectionRange(900.0f),
    spellFrameSize(64, 88),
    spellFrameCount(11),
    spellAnimDuration(0.5f),
    attackActionTriggered(false),
    damageDealtInCurAttack(false)
{
    baseScale = 1.75f;
    hurtDuration = 0.3f;
    loadSpecificAssets();
    setState(EnemyState::Idle); // Set initial state after loading assets
    attackCooldownTimer.restart();
}

void Wizard::loadSpecificAssets() {
    // Load Wizard-specific animations
    for (const auto& pair : wizardAnimSheetInfos) {
        loadStateFrames(pair.first, pair.second);
    }

    // Define local hitbox based on frame size (231x190)
    float hitboxWidth = 60.f;
    float hitboxHeight = 90.f;
    float hitboxLeft = (231.f - hitboxWidth) / 2.f;
    float hitboxTop = 190.f - hitboxHeight - 48.f; // Adjust to fit the sprite base
    localHitbox = sf::FloatRect(hitboxLeft, hitboxTop, hitboxWidth, hitboxHeight);

    // Set initial sprite texture and rect from Idle state
    if (animations.count(EnemyState::Idle) && !animations[EnemyState::Idle].frames.empty()) {
        sprite.setTexture(*animations[EnemyState::Idle].sheet);
        sprite.setTextureRect(animations[EnemyState::Idle].frames[0]);
        adjustSpriteOriginAndScale(); // Call this after texture is set
    }
    else {
        std::cerr << "Wizard Idle animation not loaded, sprite texture not set." << std::endl;
    }

    // Load lightning texture
    if (!lightningTexture.loadFromFile(baseAssetPath + "/Lightning.png")) {
        std::cerr << "Failed to load lightning texture!\n";
    }
}

void Wizard::loadStateFrames(EnemyState state, const AnimSheetInfo& info) {
    Animation anim;
    anim.frameDuration = info.frameDuration;
    anim.loop = info.loop;

    std::string sheetPath = baseAssetPath + "/" + info.filename;
    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile(sheetPath)) {
        std::cerr << "Failed to load Wizard sheet: " << sheetPath << std::endl;
        return;
    }
    anim.sheet = texture;

    for (int i = 0; i < info.frameCount; ++i) {
        anim.frames.push_back(sf::IntRect(i * info.frameWidth, 0, info.frameWidth, info.frameHeight));
    }
    animations[state] = anim;
}

void Wizard::update(float deltaTime, Player& player, const CollisionLayer& collisionLayer) {
    // Update spell effect first
    if (lightningStrike) {
        lightningStrike->update(deltaTime);
        // Check for collision if the spell is active and hasn't dealt damage yet
        if (lightningStrike->isActive() && !lightningStrike->hasDealtDamage()) {
            if (lightningStrike->getHitBox().intersects(player.getHitBox())) {
                player.takeDamage(10); // Spell damage
                lightningStrike->setDamageDealt(true); // Mark as dealt
                std::cout << "Player hit by Wizard spell!" << std::endl;
            }
        }
        // Clean up inactive spell
        if (!lightningStrike->isActive()) {
            lightningStrike.reset();
        }
    }
    // Now call the base Enemy update for normal behavior
    Enemy::update(deltaTime, player, collisionLayer);

}

// movement and attack logic
void Wizard::updateAI(float deltaTime, Player& player) {
    // Prevent attacking if player is dead (hitbox is zero)
	sf::FloatRect playerHitBox = player.getHitBox();
	sf::Vector2f playerPos = player.getPosition();
    if (player.isDead()) {
        setState(EnemyState::Idle);
        velocity.x = 0;
        return;
    }

    float distanceToPlayer = magnitude(playerPos - position); // True distance


    // Basic AI: update facing direction, then decide to run or attack
    updateFacingDirection(playerPos);

    if (currentState == EnemyState::Attacking || currentState == EnemyState::Attacking2) {
		velocity.x = 0; // Stop moving during attack animation
        // The base animate() will transition to Idle when attack anim finishes if it's non-looping.
        // performAttackLogic is responsible for the actual shot during this state if cooldown allows.
        performAttackLogic(player); // Check if it can attack again
        return; // Don't change state or move if in attack animation
    }

    // If we are not in the attacking state, the flag from any previous attack must be reset.
    attackActionTriggered = false;
    damageDealtInCurAttack = false;

    // If player is out of detection range, go idle.
    if (distanceToPlayer > detectionRange) {
        setState(EnemyState::Idle);
        velocity.x = 0;
        return;
    }
    bool canAttack = attackCooldownTimer.getElapsedTime().asSeconds() >= (currentState == EnemyState::Attacking2 ? meleeInterval : spellInterval);
    // 1st Priority: Melee attack if player is close enough
    if (distanceToPlayer < meleeRange && canAttack) {
        setState(EnemyState::Attacking2);
        attackCooldownTimer.restart();
    }
    // 2nd Priority: Cast spell if player is within attack range
    else if (distanceToPlayer < spellRange && canAttack) {
        setState(EnemyState::Attacking);
        attackCooldownTimer.restart(); // Restart timer because we just initiated an attack
    }
    else if (distanceToPlayer > meleeRange && distanceToPlayer < detectionRange) {
        setState(EnemyState::Running);
        velocity.x = direction * speed; //*deltaTime; // Move in the facing direction
    }
    // Priority 4: Stand idle if close but attack is on cooldown
    else {
        setState(EnemyState::Idle);
        velocity.x = 0;
    }
}

void Wizard::performAttackLogic(Player& player) {
    // This is called every frame of the attack animation.
    if (currentState == EnemyState::Attacking) {
        const int SPELL_CAST_FRAME = 4;
        if (currentFrame == SPELL_CAST_FRAME && !attackActionTriggered) {
            // Only create spell, checking collision will be done in CheckSpellCollision
            sf::Vector2f lightningPos = player.getPosition();
            lightningPos.y -= spellFrameSize.y * 1.5f; // Position above the player
            lightningStrike = std::make_unique<SpellEffect>(
                lightningTexture, lightningPos, spellFrameSize, spellFrameCount, spellAnimDuration
            );
            attackActionTriggered = true; // Only create the spell effect once per attack animation
			std::cout << "Wizard casts a spell at player!\n";
        }
    }
    // We only register the hit ONCE per animation.
    else if (currentState == EnemyState::Attacking2)
    {
        const int MELEE_HIT_FRAME = 5;
        if (currentFrame == MELEE_HIT_FRAME && !damageDealtInCurAttack) {
            if (getHitBox().intersects(player.getHitBox())) {
                player.takeDamage(10);
                damageDealtInCurAttack = true;
                std::cout << "Wizard melee hit player!\n";
            }
        }
    }
    // The flag will be reset in updateAI once the state changes away from Attacking.
}

// Override the draw method to include the lightning strike effect
void Wizard::draw(sf::RenderWindow& window) {
    // Call base class draw to draw the Wizard's sprite
    Enemy::draw(window);

    if (lightningStrike && lightningStrike->isActive()) {
        lightningStrike->draw(window);
    }
}


sf::FloatRect Wizard::getHitBox() const {
    //// Kích thước gốc của frame là 231x190
    return sprite.getTransform().transformRect(localHitbox);
}