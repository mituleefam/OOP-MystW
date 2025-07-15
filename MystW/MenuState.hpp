#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Button.hpp"

class MenuState : public State
{
private:
	sf::Font font1, font2;
	sf::Text text;
	Button exit, trophy, play, sound;
	sf::Texture trophyTex, soundTex, menuB;
	sf::Sprite menuBackground;
	StateManager* states;
	sf::RenderWindow* win;
public:
	MenuState(StateManager* sm, sf::RenderWindow * window );
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow &window) override;
};
