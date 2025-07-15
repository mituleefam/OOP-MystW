#include "GameState.hpp"
#include "MenuState.hpp"
#include "PausePage.hpp"
#include <iostream>

GameState::GameState(StateManager* sm, sf::RenderWindow* window):win(window), states(sm)
{
	background.loadStage("image/backgroundGame", *win);
	pauseTex.loadFromFile("image/icon/pause_icon.png");
	sf::Vector2f viewSize = win->getView().getSize();
	float scaleX = viewSize.x / 800.f;
	float scaleY = viewSize.y / 600.f;

	pause.setIcon(pauseTex, 15.f * scaleX, 15.f * scaleY, 24.0f * scaleX);
	pause.setShape(15.f * scaleX, 15.f * scaleY, 25.f * scaleX, 30.f * scaleY);



	//Hoang's part
	const int tileWidth = 32;
	const int tileHeight = 32;
	const unsigned int TILE_SIZE = 32;
	// Load tileset

	if (!tileSet.load("image/tile/tileset.png", "map.csv", TILE_SIZE)) {
		std::cout << "Failed to load tileset!" << std::endl;
	}


	// Load collision layer
	collisionLayer.load("Collision.csv", TILE_SIZE);
}

void GameState::handleEvent(sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::A)
			isAClicked = true;

		if (event.key.code == sf::Keyboard::D)
			isDClicked = true;
	}
	if (pause.isClicked(*win, event) || event.key.code == sf::Keyboard::Escape)
	{
		std::cout << "Pause is clicked \n";
		states->pushState(std::make_unique<PausePage>(states,win));
	}
}

void GameState::update(float delta)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || isAClicked)
		background.update(delta, 'A');
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || isDClicked)
		background.update(delta, 'D');

	pause.applyHoverEffect(*win);

	isAClicked = false;
	isDClicked = false;
}

void GameState::render(sf::RenderWindow& window)
{
	window.draw(tileSet);
	background.draw(window);
	pause.render(window);

}