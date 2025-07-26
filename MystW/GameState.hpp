#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Camera.h"
#include "Background.hpp"
#include "Button.hpp"
#include "TileSets.h"
#include "CollisionLayer.hpp"
#include "Player.h" // ADDED: Include the Player header
#include "Enemy.h"  // ADDED: Include the base Enemy header
#include <vector>   // ADDED: To hold enemies
#include <memory>   // ADDED: For smart pointers

class GameState : public State
{
private:
	ManageLayer background;
	sf::RenderWindow* win;
	StateManager* states;
	bool isAClicked = false, isDClicked = false;
	Button pause;
	sf::Texture pauseTex;


	//Hoang's properies
	CollisionLayer collisionLayer;
	Tileset tileSet;

	// --- ADDED: Game Entities ---
	Player player; // The main player object
	std::vector<std::unique_ptr<Enemy>> enemies; // A list to hold all enemies

	//---ADDED:Camera ---
	Camera camera;

public:
	GameState(StateManager* sm, sf::RenderWindow* window);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;

	// ADDED: Helper function to load enemies
	void loadEnemies();
};