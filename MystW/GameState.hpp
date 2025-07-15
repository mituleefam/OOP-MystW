#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Background.hpp"
#include "Button.hpp"
#include "TileSets.h"
#include "CollisionLayer.hpp"

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
public:
	GameState(StateManager* sm, sf::RenderWindow* window);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;
};