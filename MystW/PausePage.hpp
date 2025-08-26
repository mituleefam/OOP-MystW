#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Button.hpp"

class PausePage : public State
{
private:
	Button resume, back;
	sf::Texture resTex, backTex;
	sf::RectangleShape overlay, pauseBox;

	sf::RenderWindow* win;
	StateManager* states;

	bool isResumeClicked = false;
	bool isHomeClicked = false;
public:
	//PausePage(StateManager* sm, sf::RenderWindow* window);
	PausePage(StateManager* sm);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;
};