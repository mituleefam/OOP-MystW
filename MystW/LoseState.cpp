#include "LoseState.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include <iostream>
LoseState::LoseState(StateManager* sm) : states(sm)
{
	win = states->getWindow();
	if (!font.loadFromFile("font/bitcount.ttf"))
		std::cout << "Cannot load file font bitcount! \n";
	text.setFont(font);
	text.setString("YOU LOSE!");
	text.setCharacterSize(100);
	sf::FloatRect textBound = text.getLocalBounds();
	text.setOrigin(textBound.width / 2, textBound.height / 2);

	sf::Vector2f center(1920.f / 2.f, 1080.f / 2.f);

	text.setPosition(center.x, center.y - 150);
	text.setFillColor(sf::Color(255, 60, 60));
	text.setOutlineColor(sf::Color::White);
	text.setOutlineThickness(2.f);

	back.setShape(center.x + 100, center.y, 200, 150);
	back.setText(font, "HOME");

	restart.setShape(center.x - 320, center.y, 350, 150);
	restart.setText(font, "RESTART");

	overlay.setSize(sf::Vector2f(1920.f, 1080.f));
	overlay.setFillColor(sf::Color(0, 0, 0, 120));

	loseBox.setSize(sf::Vector2f(960.f, 540.f));
	loseBox.setFillColor(sf::Color(44, 44, 44, 240));
	loseBox.setOutlineColor(sf::Color(178, 34, 34));
	loseBox.setOutlineThickness(4.f);
	loseBox.setOrigin(480.f, 270.f);
	loseBox.setPosition(center);
}

void LoseState::handleEvent(sf::Event& event)
{
	if (back.isClicked(*win, event))
		isHomeClicked = true;

	if (restart.isClicked(*win, event))
		isRestartClicked = true;
}

void LoseState::update(float delta)
{
	std::string hoverColor = "FF5E5E";
	std::string buttonColor = "444444";
	restart.applyHoverEffect(*win, hoverColor, buttonColor);
	back.applyHoverEffect(*win, hoverColor, buttonColor);
	if (isRestartClicked)
	{
		isRestartClicked = false;
		states->pushState(std::make_unique<GameState>(states));
		AudioManager::getInstance()->playGameMusic1();
		return;
	}

	if (isHomeClicked)
	{
		states->pushState(std::make_unique<MenuState>(states));
		AudioManager::getInstance()->playMenuMusic();
		isHomeClicked = false;
		return;
	}
}

void LoseState::render(sf::RenderWindow& window)
{
	win->setView(*states->getUiView());
	win->draw(overlay);
	win->draw(loseBox);
	restart.render(*win);
	back.render(*win);
	win->draw(text);
}