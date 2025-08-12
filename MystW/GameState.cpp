#include "GameState.hpp"
#include "MenuState.hpp"
#include "Camera.h"
#include "TileSets.h"
#include "PausePage.hpp"
#include <iostream>
// ADDED: Include specific enemy types you want to create
#include "EnemyFactory.h"

GameState::GameState(StateManager* sm): states(sm)
{
	win = states->getWindow();
	float maxHP = 100;
	background.loadStage("image/backgroundGame", *win);
	pauseTex.loadFromFile("image/icon/pause_icon.png");
	sf::Vector2f viewSize = (*states->getUiView()).getSize();
	float scaleX = viewSize.x / 800.f;
	float scaleY = viewSize.y / 600.f;

	pause.setIcon(pauseTex, 15.f * scaleX, 15.f * scaleY, 24.0f * scaleX);
	pause.setShape(15.f * scaleX, 15.f * scaleY, 25.f * scaleX, 30.f * scaleY);

	sf::FloatRect textRect = timerText.getLocalBounds();
	timerText.setOrigin(textRect.width / 2.0f, textRect.height / 2.0f);
	timerText.setPosition(300.f * scaleX, 15.f * scaleY);

	if (!font.loadFromFile("font/Merriweather-VariableFont_opsz,wdth,wght.ttf"))
		std::cout << "cannot load font MerriWeather \n";
	hp = new HealthBar(maxHP, sf::Vector2f(150, 50), sf::Vector2f(200, 20), &font);

	timerText.setFont(font);
	timerText.setCharacterSize(50);
	timerText.setFillColor(sf::Color::Red);

	const unsigned int TILE_SIZE = 32;

	// Load tileset trước để lấy kích thước
	if (!tileSet.load("image/tile/tileset.png", "map1.csv", TILE_SIZE, win->getView().getSize())) {
		std::cout << "Failed to load tileset!" << std::endl;
	}

	// Load collision
	collisionLayer.load("Collision1.csv", TILE_SIZE);

	// Bây giờ mới lấy được kích thước map từ tileset
	float mapWidth = tileSet.getMapWidth() * TILE_SIZE;
	float mapHeight = tileSet.getMapHeight() * TILE_SIZE;

	camera = Camera(viewSize, sf::Vector2f(mapWidth, mapHeight)); // Gán lại sau khi có kích thước

	// BẮT ĐẦU GAME VỚI LEVEL 1
	loadLevel(1);

	// Player
	player.Initialize();
	player.Load();
	player.setPosition(200, 600);

	loadEnemies();

	// Score
	font.loadFromFile("font/cursive.ttf");
	scoreText.setFont(font);
	scoreText.setCharacterSize(64);
	scoreText.setFillColor(sf::Color::White);
	// Đặt vị trí cho text ở góc trên bên phải
	scoreText.setPosition(1920.f - 300.f, 20.f);

	// CẤU HÌNH CHO DEBUG HITBOX
	debugHitbox.setFillColor(sf::Color::Transparent);
	debugHitbox.setOutlineColor(sf::Color::Red);
	debugHitbox.setOutlineThickness(2.0f);
	// Cấu hình cho điểm debug
	debugCollisionPoint.setRadius(4.f);
	debugCollisionPoint.setFillColor(sf::Color::Green);
	debugCollisionPoint.setOrigin(4.f, 4.f);
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
	if (scoreManager.getScore() == 100)
	{
		states->pushState(std::make_unique<WinState>(states));
		AudioManager::getInstance()->playWinMusic();
	}
	if (player.health == 0)
	{
		states->pushState(std::make_unique<LoseState>(states));
		AudioManager::getInstance()->playLoseMusic();
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
	sf::Time elapsed = gameClock.getElapsedTime();
	int totalSeconds = static_cast<int>(elapsed.asSeconds());
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << minutes << ":"
		<< std::setw(2) << std::setfill('0') << seconds;
	timerText.setString(ss.str());

	if (player.isAttacked == true)
	{
		hp->setCurrentHealth(player.health);
		player.isAttacked = false;
	}

	for (auto it = enemies.begin(); it != enemies.end(); ) {
		Enemy* currentEnemy = it->get(); // Get raw pointer for convenience

		//currentEnemy->update(delta, player.getPosition(), player.getHitBox(), collisionLayer);
		currentEnemy->update(delta, player, collisionLayer);

		if (currentEnemy->isDead()) { // isDead() should mean "animation finished and can be removed"
			if (currentLevel == 1)
				scoreManager.incrementScore(5);
			else if (currentLevel == 2)
				scoreManager.incrementScore(10);
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
			//if (Elf* elf = dynamic_cast<Elf*>(currentEnemy)) {
			//	if (elf->checkArrowCollisions(player.getHitBox())) {
			//		player.takeDamage(5);
			//		elf->removeArrowsCollidingWith(player.getHitBox());
			//		std::cout << "Player hit by Elf arrow!" << std::endl;
			//	}
			//}
			//if (Striker* striker = dynamic_cast<Striker*>(currentEnemy)) {
			//	if (striker->attackRegistered && !striker->damageDealtThisAttack) {
			//		player.takeDamage(5);
			//		striker->damageDealtThisAttack = true;
			//		std::cout << "Player hit by Striker!" << std::endl;
			//	}
			//}
			//if (Wizard* wizard = dynamic_cast<Wizard*>(currentEnemy)) {
			//	if (wizard->checkSpellCollisions(player.getHitBox())) {
			//		player.takeDamage(10); // Ví dụ sát thương của spell
			//		wizard->onSpellHit(); // cancel the spell right after it hits
			//	}
			//	else if (wizard->getCurrentState() == EnemyState::Attacking2 && wizard->attackRegistered) {
			//		// Giả sử đòn cận chiến cũng chỉ gây sát thương 1 lần
			//		// (bạn sẽ cần thêm cờ damageDealtThisAttack cho wizard nếu cần)
			//		player.takeDamage(5);
			//		wizard->attackRegistered = false; // Tạm thời reset ngay để tránh loop
			//	}
			//}
			// The enemy now damages the player from within its own updateAI function.
			// No need for dynamic_cast or calling dealDamage here.
			++it;
		}
	}
	if (enemies.empty() && currentLevel <= 2) {
		// All enemies defeated, trigger level transition
		loadNextLevel();
	}
	pause.applyHoverEffect(*win);
	scoreText.setString("Score: " + std::to_string(scoreManager.getScore()));
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
		// VẼ HITBOX CỦA ENEMY RA MÀN HÌNH
		sf::FloatRect hb = enemy->getHitBox();
		debugHitbox.setSize(sf::Vector2f(hb.width, hb.height));
		debugHitbox.setPosition(hb.left, hb.top);
		window.draw(debugHitbox);

		// === VẼ CÁC ĐIỂM KIỂM TRA VA CHẠM ===
		// Điểm bên phải
		debugCollisionPoint.setPosition(hb.left + hb.width, hb.top); window.draw(debugCollisionPoint);
		debugCollisionPoint.setPosition(hb.left + hb.width, hb.top + hb.height / 2); window.draw(debugCollisionPoint);
		debugCollisionPoint.setPosition(hb.left + hb.width, hb.top + hb.height); window.draw(debugCollisionPoint);
		// Điểm bên trái
		debugCollisionPoint.setPosition(hb.left, hb.top); window.draw(debugCollisionPoint);
		debugCollisionPoint.setPosition(hb.left, hb.top + hb.height / 2); window.draw(debugCollisionPoint);
		debugCollisionPoint.setPosition(hb.left, hb.top + hb.height); window.draw(debugCollisionPoint);
	}

	// Draw UI on top of everything
	window.setView(*states->getUiView()); // << ĐÂY LÀ BƯỚC QUAN TRỌNG NHẤT
	pause.render(window);
	hp->draw(window);
	window.draw(timerText);
	// Ví dụ sau này có thể vẽ thêm:
	// scoreText.draw(window);
	window.draw(scoreText);
	// healthBar.draw(window);
}

// ADDED: Implementation for loading enemies
void GameState::loadEnemies() {
	enemies.clear();
	if (currentLevel == 1)
	{
		enemies.emplace_back(EnemyFactory::createEnemy("Elf", 2000.0f, 900.0f));
		enemies.emplace_back(EnemyFactory::createEnemy("Striker", 3000.0f, 900.0f));
		enemies.emplace_back(EnemyFactory::createEnemy("Wizard", 2400.0f, 900.0f));
		//enemies.emplace_back(EnemyFactory::createEnemy("LeafBoss", 2400.0f, 900.0f));
		//enemies.emplace_back(EnemyFactory::createEnemy("WaterBoss", 2400.0f, 900.0f));
	}
	else if (currentLevel == 2)
	{
		enemies.emplace_back(EnemyFactory::createEnemy("Wizard", 1000.0f, 900.0f));
	}
	else if (currentLevel == 3)
	{
		enemies.emplace_back(EnemyFactory::createEnemy("LeafBoss", 2400.0f, 900.0f));
		enemies.emplace_back(EnemyFactory::createEnemy("WaterBoss", 2400.0f, 900.0f));
	}
	// Add more as needed
	std::cout << "Enemies loaded for level " << currentLevel << std::endl;
}

void GameState::loadLevel(int level) {
	currentLevel = level;

	// Tạo tên file map dựa trên level
	std::string mapFile = "map" + std::to_string(level) + ".csv";
	std::string collisionFile = "Collision" + std::to_string(level) + ".csv";

	// Load map và collision cho level mới
	if (!tileSet.load("image/tile/tileset.png", mapFile, 32, win->getView().getSize())) {
		std::cout << "Failed to load tileset for level " << level << std::endl;
	}
	collisionLayer.load(collisionFile, 32);

	// Reset vị trí người chơi
	player.setPosition(200, 600); // Hoặc vị trí bắt đầu của từng màn

	// Reset camera
	float mapWidth = tileSet.getMapWidth() * 32;
	float mapHeight = tileSet.getMapHeight() * 32;
	camera = Camera(win->getView().getSize(), sf::Vector2f(mapWidth, mapHeight));

	// Load enemy cho level mới
	loadEnemies();
}

void GameState::loadNextLevel() {
	const int MAX_LEVELS = 2; // Ví dụ game của bạn có 2 level

	if (currentLevel < MAX_LEVELS) {
		// Load level tiếp theo
		loadLevel(currentLevel + 1);
	}
	else {
		// Đã hoàn thành tất cả các level!
		std::cout << "Congratulations! You have completed the game!" << std::endl;

		// Lưu điểm cuối cùng vào high score
		ScoreManager::addScore(scoreManager.getScore());

		// Quay về Menu chính
		states->popState(); // Xóa GameState hiện tại
		// Tùy chọn: có thể push một state "You Win" vào đây
	}
}