#include "GameState.hpp"
#include "MenuState.hpp"
#include "Camera.h"
#include "TileSets.h"
#include "PausePage.hpp"
#include "AudioManager.hpp"  // ADDED: Missing include for AudioManager
#include <iostream>
#include "EnemyFactory.h"
#include <sstream>           // ADDED: Missing include for stringstream
#include <iomanip>           // ADDED: Missing include for setw/setfill

GameState::GameState(StateManager* sm): states(sm)
{
	win = states->getWindow();
	float maxHP = 100;
	// XÓA PHẦN LOAD BACKGROUND Ở ĐÂY, VÌ NÓ SẼ ĐƯỢC LOAD TRONG loadLevel()
	//background.loadStage("image/backgroundGame", *win);
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

	// Player
	player.Initialize();
	player.Load();

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

	// START WITH LEVEL 1
	loadLevel(1);
}

void GameState::handleEvent(sf::Event& event)
{
	// This function primarily handles events that happen once, like key presses
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			states->pushState(std::make_unique<PausePage>(states));
			return; // FIXED: Prevent duplicate pause state pushes
		}
	}

	// FIXED: Removed duplicate escape key check
	if (pause.isClicked(*win, event))
	{
		std::cout << "Pause is clicked \n";
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
	// 1. Update tất cả enemy trước
	for (auto& enemy : enemies) {
		enemy->update(delta, player, collisionLayer);
	}

	// 2. Update player
	player.Update(delta, collisionLayer);

	// 3. Update Spirit (dùng vị trí player mới và enemy mới nhất)
	player.getSpirit().update(delta, player.getPosition(), enemies);

	// 4. Giữ player trong map
	sf::Vector2f pos = player.getPosition();
	if (pos.x < 0.f) {
		player.setPosition(0.f, pos.y);
	}

	// 5. Update camera và background
	camera.follow(player.getPosition());
	background.update(camera.getView().getCenter());

	// 6. Update timer text
	sf::Time elapsed = gameClock.getElapsedTime();
	int totalSeconds = static_cast<int>(elapsed.asSeconds());
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << minutes << ":"
		<< std::setw(2) << std::setfill('0') << seconds;
	timerText.setString(ss.str());

	// 7. Update máu player khi bị tấn công
	if (player.isAttacked) {
		hp->setCurrentHealth(player.health);
		player.isAttacked = false;
	}

	// 8. Xử lý enemy chết hoặc bị player đánh
	for (auto it = enemies.begin(); it != enemies.end();) {
		Enemy* currentEnemy = it->get();

		if (currentEnemy->isDead()) {
			if (currentLevel == 1)
				scoreManager.incrementScore(5);
			else if (currentLevel == 2)
				scoreManager.incrementScore(10);
			it = enemies.erase(it);
			std::cout << "Enemy removed from game." << std::endl;
		}
		else {
			if (player.isAttacking && !player.attackRegistered &&
				!currentEnemy->checkIsHurting() &&
				player.getAttackBounds().intersects(currentEnemy->getHitBox()))
			{
				currentEnemy->takeDamage(1);
				player.attackRegistered = true;
				std::cout << "Player attacked enemy!" << std::endl;
			}
			++it;
		}
	}

	// 9. Check qua màn
	if (enemies.empty()) {
		float mapWidthPixels = tileSet.getMapWidth() * 32;
		if (player.getRightEdge() >= mapWidthPixels - 0.5f) {
			std::cout << "Trigger next level!" << std::endl;
			loadNextLevel();
		}
	}

	// 10. Update UI
	pause.applyHoverEffect(*win);
	scoreText.setString("Score: " + std::to_string(scoreManager.getScore()));
}


void GameState::render(sf::RenderWindow& window)
{
	// 1. DRAW GAME WORLD
	// window.clear();
	window.setView(*states->getUiView()); // << ĐÂY LÀ BƯỚC QUAN TRỌNG NHẤT
	background.draw(window);
	camera.applyTo(window);
	// Draw order is important: background first, then map, then entities
	window.draw(tileSet);

	// --- ADDED: Draw the player and enemies ---
	player.Draw(window); // Assuming Player has a Draw method that draws its sprite on the window
	player.getSpirit().draw(window);
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

	// 2. Draw UI on top of everything
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
		enemies.emplace_back(EnemyFactory::createEnemy("Elf", 2400.0f, 900.0f));
		//enemies.emplace_back(EnemyFactory::createEnemy("Striker", 3000.0f, 900.0f));
	}
	else if (currentLevel == 2)
	{
		enemies.emplace_back(EnemyFactory::createEnemy("Wizard", 2400.0f, 900.0f));
	}
	else if (currentLevel == 3)
	{
		//enemies.emplace_back(EnemyFactory::createEnemy("LeafBoss", 2400.0f, 900.0f));
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
	std::string backgroundPath = "image/backgroundGame" + std::to_string(level);
	background.loadStage(backgroundPath, *win);
	// Load map và collision cho level mới
	std::string tileSetFile = "image/tile/tileset" + std::to_string(level) + ".png";
	if (!tileSet.load(tileSetFile, mapFile, 32, win->getView().getSize())) {
		std::cout << "Failed to load tileset for level " << level << std::endl;
		// Có thể xử lý lỗi ở đây, ví dụ quay về menu
		states->popState();
		return;
	}
	collisionLayer.load(collisionFile, 32);

	// Reset vị trí người chơi
	player.setPosition(200, 600); // Hoặc vị trí bắt đầu của từng màn

	// Reset camera
	float mapWidth = tileSet.getMapWidth() * 32;
	float mapHeight = tileSet.getMapHeight() * 32;
	camera = Camera(win->getView().getSize(), sf::Vector2f(mapWidth, mapHeight));

	//LoadSpirit
	if (level == 3) {
		player.getSpirit().activate();
	}

	// Load enemy cho level mới
	loadEnemies();
}

void GameState::loadNextLevel() {
	const int MAX_LEVELS = 3;

	if (currentLevel < MAX_LEVELS) {
		if (currentLevel == 1) {
			loadLevel(currentLevel + 1);
		}
		else if (currentLevel == 2) {
			loadLevel(currentLevel + 1);
			states->pushState(std::make_unique<RewardState>(states, &player.getSpirit()));
		}
	}
	else {
		std::cout << "Congratulations! You have completed the game!" << std::endl;
		ScoreManager::addScore(scoreManager.getScore());
		states->pushState(std::make_unique<WinState>(states));
		AudioManager::getInstance()->playWinMusic();
	}
}
