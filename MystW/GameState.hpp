#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Camera.h"
#include "Background.hpp"
#include "Button.hpp"
#include "TileSets.h"
#include "CollisionLayer.hpp"
#include "RewardState.h"
#include "Player.h" // ADDED: Include the Player header
#include "Enemy.h"  // ADDED: Include the base Enemy header
#include <vector>   // ADDED: To hold enemies
#include <memory>   // ADDED: For smart pointers
#include "ScoreManager.hpp"
#include "WinState.hpp"
#include "LoseState.hpp"
#include "HealthBar.h"
#include <sstream>
#include <iomanip>
class GameState : public State
{
private:
	ManageLayer background;
	sf::RenderWindow* win;
	StateManager* states;
	bool isAClicked = false, isDClicked = false;
	Button pause;
	sf::Texture pauseTex;
	sf::Font font;
	sf::Text timerText;
	sf::Clock gameClock;
	HealthBar* hp;


	//Hoang's properies
	CollisionLayer collisionLayer;
	Tileset tileSet;
	// --- ADDED: Game Entities ---
	Player player; // The main player object
	std::vector<std::unique_ptr<Enemy>> enemies; // A list to hold all enemies

	//---ADDED:Camera ---
	Camera camera;

	// Level
	int currentLevel = 1; // Track current level

	// Score
	ScoreManager scoreManager;
	sf::Text scoreText;

	//sf::View gameView;
	sf::Texture groundTexture;
	sf::Sprite groundSprite;

	sf::RectangleShape debugHitbox;
	sf::CircleShape debugCollisionPoint;
public:
	GameState(StateManager* sm);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;

	// ADDED: Helper function to load enemies
	void loadEnemies();
	// Level
	void loadLevel(int level);
	void loadNextLevel();
};