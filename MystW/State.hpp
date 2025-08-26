#pragma once
#include <SFML/Graphics.hpp>
class State
{
public:
	virtual ~State() {};

	virtual void handleEvent(sf::Event& event) = 0;
	virtual void update(float delta) = 0;
	virtual void render(sf::RenderWindow& window) = 0;

	virtual bool isFinished() const { return false; }
};