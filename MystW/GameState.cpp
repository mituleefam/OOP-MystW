#include "GameState.hpp"
#include "MenuState.hpp"
#include "Camera.h"
#include "TileSets.h"
#include "PausePage.hpp"
#include <iostream>
// ADDED: Include specific enemy types you want to create
#include "Elf.h"
#include "Striker.h"

GameState::GameState(StateManager* sm, sf::RenderWindow* window): win(window), states(sm)
{
	background.loadStage("image/backgroundGame", *win);
	pauseTex.loadFromFile("image/icon/pause_icon.png");

	sf::Vector2f viewSize = window->getView().getSize();
	float scaleX = viewSize.x / 800.f;
	float scaleY = viewSize.y / 600.f;

	pause.setIcon(pauseTex, 15.f * scaleX, 15.f * scaleY, 24.0f * scaleX);
	pause.setShape(15.f * scaleX, 15.f * scaleY, 25.f * scaleX, 30.f * scaleY);

	const unsigned int TILE_SIZE = 32;

	// Load tileset trước để lấy kích thước
	if (!tileSet.load("image/tile/tileset.png", "map1.csv", TILE_SIZE, window->getView().getSize())) {
		std::cout << "Failed to load tileset!" << std::endl;
	}

	// Load collision
	collisionLayer.load("Collision1.csv", TILE_SIZE);

	// Bây giờ mới lấy được kích thước map từ tileset
	float mapWidth = tileSet.getMapWidth() * TILE_SIZE;
	float mapHeight = tileSet.getMapHeight() * TILE_SIZE;

	camera = Camera(viewSize, sf::Vector2f(mapWidth, mapHeight)); // Gán lại sau khi có kích thước

	// Player
	player.Initialize();
	player.Load();
	player.setPosition(200, 600);

	loadEnemies();
}




void GameState::handleEvent(sf::Event& event)
{

	// This function primarily handles events that happen once, like key presses
	if (event.type == sf::Event::KeyPressed)
	{
		// Removed isAClicked and isDClicked, as we will handle continuous movement in update()
		if (event.key.code == sf::Keyboard::Escape)
		{
			// states->pushState(std::make_unique<PausePage>(states, win));
			states->pushState(std::make_unique<PausePage>(states));
		}
	}

	if (pause.isClicked(*win, event) || event.key.code == sf::Keyboard::Escape)
	{
		std::cout << "Pause is clicked \n";
		// states->pushState(std::make_unique<PausePage>(states,win));
		states->pushState(std::make_unique<PausePage>(states));
	}
}

void GameState::update(float delta)
{
	// --- Update Player ---
	player.Update(delta, collisionLayer);
	sf::Vector2f pos = player.getPosition();
	if (pos.x < 0.f) {
		player.setPosition(0.f, pos.y); // hoặc player.setX(pos.x);
	}

	camera.follow(player.getPosition());
	// --- Link Player Movement to Background Scrolling ---
	// The background should scroll opposite to the player's movement
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || isAClicked)
	//	background.update(delta, 'A');
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || isDClicked)
	//	background.update(delta, 'D');
	// GỌI HÀM UPDATE MỚI CỦA BACKGROUND
	// Truyền vào tâm của camera
	background.update(camera.getView().getCenter());

	for (auto it = enemies.begin(); it != enemies.end(); ) {
		Enemy* currentEnemy = it->get(); // Get raw pointer for convenience

		currentEnemy->update(delta, player.getPosition(), player.getHitBox());

		if (currentEnemy->isDead()) { // isDead() should mean "animation finished and can be removed"
			it = enemies.erase(it);
			std::cout << "Enemy removed from game." << std::endl;
		}
		else {
			// Player attacks enemy
			if (player.isAttacking && !player.attackRegistered && !currentEnemy->checkIsHurting() && player.getAttackBounds().intersects(currentEnemy->getHitBox())) {
				currentEnemy->takeDamage(1); // Example damage amount
				player.attackRegistered = true;
				std::cout << "Player attacked enemy!" << std::endl;
				// Death check after player attack is implicitly handled by enemy's own update/isDead next iteration
			}

			// Enemy attacks player
			if (Elf* elf = dynamic_cast<Elf*>(currentEnemy)) {
				if (elf->checkArrowCollisions(player.getHitBox())) {
					player.takeDamage(5);
					elf->removeArrowsCollidingWith(player.getHitBox());
					std::cout << "Player hit by Elf arrow!" << std::endl;
				}
			}
			if (Striker* striker = dynamic_cast<Striker*>(currentEnemy)) {
				if (striker->attackRegistered) {
					player.takeDamage(5);
					std::cout << "Player hit by Striker!" << std::endl;
				}
			}
			++it;
		}
	}
	pause.applyHoverEffect(*win);

	//isAClicked = false;
	//isDClicked = false;
}

void GameState::render(sf::RenderWindow& window)
{
	// window.clear();
	window.setView(*states->getUiView()); // << ĐÂY LÀ BƯỚC QUAN TRỌNG NHẤT
	background.draw(window);
	// ---ADDED: Camera Character ---
	camera.applyTo(window);
	// Draw order is important: background first, then map, then entities
	window.draw(tileSet);

	// --- ADDED: Draw the player and enemies ---
	player.Draw(window); // Assuming Player has a Draw method that draws its sprite on the window
	for (const auto& enemy : enemies) {
		enemy->draw(window);
	}

	// Draw UI on top of everything
	window.setView(*states->getUiView()); // << ĐÂY LÀ BƯỚC QUAN TRỌNG NHẤT
	pause.render(window);
	// Ví dụ sau này có thể vẽ thêm:
	// scoreText.draw(window);
	// healthBar.draw(window);
}

// ADDED: Implementation for loading enemies
void GameState::loadEnemies() {
	// This is where you can create various enemies for the level
	// Example: Create an Elf and a Striker
	// Note: You need to include "Elf.h" and "Striker.h"
	// The positions (e.g., 1000, 800) need to be adjusted to fit your map
	// enemies.emplace_back(std::make_unique<Elf>("Assets/Enemy/Elf/Textures", 1000.0f, 800.0f));
	// enemies.emplace_back(std::make_unique<Striker>("Assets/Enemy/Striker/Textures", 1200.0f, 880.0f));
	std::cout << "Enemies loaded (example code is commented out)." << std::endl;
}