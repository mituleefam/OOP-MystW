#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Button.hpp"
#include "AudioManager.hpp"

class WinState : public State
{
private:
	Button restart, back;
	sf::Texture resTex, backTex;
	sf::RectangleShape overlay, winBox;
	sf::Font font;
	sf::Text text;

	sf::RenderWindow* win;
	StateManager* states;

	bool isRestartClicked = false;
	bool isHomeClicked = false;
public:
	WinState(StateManager* sm);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;
};