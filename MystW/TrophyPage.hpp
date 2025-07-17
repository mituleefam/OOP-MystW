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

	sf::RectangleShape trophyBox;
	std::vector<sf::Text> scoreText;
	sf::Font font;

public:
	TrophyPage(StateManager* sm, sf::RenderWindow* window);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;
};

