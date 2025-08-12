#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Button.hpp"
#include "AudioManager.hpp"

class LoseState : public State
{
private:
	Button restart, back;
	sf::Texture resTex, backTex;
	sf::RectangleShape overlay, loseBox;
	sf::Font font;
	sf::Text text;

	sf::RenderWindow* win;
	StateManager* states;

	bool isRestartClicked = false;
	bool isHomeClicked = false;
public:
	LoseState(StateManager* sm);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;
};