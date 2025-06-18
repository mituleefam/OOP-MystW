#include <iostream>
#include "Player.h"
#include "Spirit.h"
// #include "Enemy.h" // Remove old Enemy.h include if it was separate from base
#include "Elf.h"    // Include Elf.h (which includes the new base Enemy.h)
// Potentially: #include "Goblin.h", etc. in the future

// ... (rest of your includes and using namespace std) ...

int main()
{
    // ... (window, player, spirit initialization) ...
    //---------- INITIALIZE ----------
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8; // Enable anti-aliasing for smoother edges
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Game", sf::Style::Default, settings);
    //---------- INITIALIZE ----------

    Player player;
    player.Initialize(); // Initialize the player
    player.Load();

    Spirit spirit;
    spirit.loadAssets();

    sf::Clock clock;

    std::vector<std::unique_ptr<Enemy>> enemies; // This remains the same! Polymorphism at work.
    // Create an Elf instance
    enemies.emplace_back(std::make_unique<Elf>("Assets/Enemy/Elf/Textures", 1800.0f, 800.0f));
    // To add another Elf:
    // enemies.emplace_back(std::make_unique<Elf>("Assets/Enemy/Elf/Textures", 1500.0f, 750.0f));
    // In the future, to add a Goblin (assuming Goblin class exists and inherits Enemy):
    // enemies.emplace_back(std::make_unique<Goblin>("Assets/Enemy/Goblin/Textures", 1000.0f, 800.0f));


    while (window.isOpen())
    {
        // ... (event polling) ...
        float deltaTime = clock.restart().asSeconds();

        // --- EXAMPLE OF ACTIVATING THE SPIRIT ---
        // This logic would be tied to a real game event, like picking up an item.
        // For testing, we can use a key press.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) && !spirit.isActive()) {
            spirit.activate();
        }

        player.Update(deltaTime);

        // The spirit's update logic is now self-contained.
        // It will do nothing if it's not active.
        spirit.update(deltaTime, player.getPosition(), enemies);

        for (auto it = enemies.begin(); it != enemies.end(); ) {
            Enemy* currentEnemy = it->get(); // Get raw pointer for convenience

            currentEnemy->update(deltaTime, player.getPosition(), player.getHitBox());

            if (currentEnemy->isDead()) { // isDead() should mean "animation finished and can be removed"
                it = enemies.erase(it);
                std::cout << "Enemy removed from game." << std::endl;
            }
            else {
                // Player attacks enemy
                if (player.playerIsAttacking() && !player.attackRegistered && !currentEnemy->checkIsHurting() && player.getAttackBounds().intersects(currentEnemy->getBounds())) {
                    currentEnemy->takeDamage(1); // Example damage amount
                    player.attackRegistered = true;
                    std::cout << "Player attacked enemy!" << std::endl;
                    // Death check after player attack is implicitly handled by enemy's own update/isDead next iteration
                }

                // Enemy (specifically Elf) arrows hit player
                // This needs to be type-checked or handled via a virtual method in Enemy
                // Option 1: Dynamic cast (safer)
                if (Elf* elf = dynamic_cast<Elf*>(currentEnemy)) {
                    if (elf->checkArrowCollisions(player.getHitBox())) {
                        player.takeDamage(10);
                        elf->removeArrowsCollidingWith(player.getHitBox());
                        std::cout << "Player hit by Elf arrow!" << std::endl;
                    }
                }
                // Option 2: Add virtual checkProjectileCollisions to base Enemy (better for many enemy types)
                // In Enemy.h: virtual bool checkProjectileCollisions(const sf::FloatRect& playerBounds) { return false; }
                //             virtual void handleProjectileCollisions(const sf::FloatRect& playerBounds) {}
                // In Elf.h: override these to use its arrow logic.
                // Then in main:
                // if (currentEnemy->checkProjectileCollisions(player.getHitBox())) {
                //     player.takeDamage(5); // Damage might need to be enemy-specific
                //     currentEnemy->handleProjectileCollisions(player.getHitBox());
                // }


                ++it;
            }
        }
        //spirit.Update(deltaTime, window, enemies, player); // Pass vector of enemies if spirit interacts with multiple

		spirit.draw(window); // Draw the spirit if active

        window.clear(sf::Color::Black);
        window.draw(player.sprite);
        for (const auto& enemy : enemies) {
            enemy->draw(window);
        }
        // spirit.Draw(window); // Uncomment when ready
        window.display();
    }
    return 0;
}