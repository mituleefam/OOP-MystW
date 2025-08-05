#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "Button.hpp"

class SoundPage : public State
{
private:
	sf::RenderWindow* win;
	StateManager* states;

	sf::RectangleShape soundBox;
	sf::RectangleShape slideBar;
	sf::RectangleShape track;
	sf::CircleShape sliderKnob;
	float curVol = 50.0f;
	float minVol = 0.f;
	float maxVol = 100.0f;
	bool isDragging = false;

	sf::Texture volOn, volOff;
	sf::Sprite volumeOn, volumeOff;

public:
	// SoundPage(StateManager* sm, sf::RenderWindow* window);
	SoundPage(StateManager* sm);
	void handleEvent(sf::Event& event) override;
	void update(float delta) override;
	void render(sf::RenderWindow& window) override;
};
