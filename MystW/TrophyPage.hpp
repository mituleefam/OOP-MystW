#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Button.hpp"
#include <vector>

class TrophyPage : public State
{
private:
	sf::RenderWindow* win;
	StateManager* states;
	Button exit;
	sf::Texture exitTex;
	sf::RectangleShape trophyBox;
	std::vector<sf::Text> scoreText;
	sf::Font font;
	bool isExitClicked = false;

public:
	// TrophyPage(StateManager* sm, sf::RenderWindow* window);
	TrophyPage(StateManager* sm);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;
};

