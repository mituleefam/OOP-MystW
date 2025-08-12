#include "WinState.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include <iostream>
WinState::WinState(StateManager* sm) : states(sm)
{
	win = states->getWindow();
	if (!font.loadFromFile("font/bitcount.ttf"))
		std::cout << "Cannot load file font bitcount! \n";
	text.setFont(font);
	text.setString("YOU WIN!");
	text.setCharacterSize(100);
	sf::FloatRect textBound = text.getLocalBounds();
	text.setOrigin(textBound.width / 2, textBound.height / 2);

	sf::Vector2f center(1920.f / 2.f, 1080.f / 2.f);

	text.setPosition(center.x, center.y - 150);
	text.setFillColor(sf::Color(255, 100, 0));
	text.setOutlineColor(sf::Color::White);
	text.setOutlineThickness(2.f);

	back.setShape(center.x + 100, center.y, 200, 150);
	back.setText(font, "HOME");

	restart.setShape(center.x - 320, center.y, 350, 150);
	restart.setText(font, "RESTART");

	overlay.setSize(sf::Vector2f(1920.f, 1080.f));
	overlay.setFillColor(sf::Color(255, 255, 255, 80));

	winBox.setSize(sf::Vector2f(960.f, 540.f));
	winBox.setFillColor(sf::Color(255, 255, 200, 230));
	winBox.setOutlineColor(sf::Color(255, 215, 0));
	winBox.setOutlineThickness(4.f);
	winBox.setOrigin(480.f, 270.f);
	winBox.setPosition(center);
}

void WinState::handleEvent(sf::Event& event)
{
	if (back.isClicked(*win, event))
		isHomeClicked = true;

	if (restart.isClicked(*win, event))
		isRestartClicked = true;
}

void WinState::update(float delta)
{
	std::string hoverColor = "FFD700";
	std::string buttonColor = "FFD8A8";
	restart.applyHoverEffect(*win, hoverColor, buttonColor);
	back.applyHoverEffect(*win, hoverColor, buttonColor);
	if (isRestartClicked)
	{
		isRestartClicked = false;
		states->pushState(std::make_unique<GameState>(states));
		AudioManager::getInstance()->playGameMusic();
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

void WinState::render(sf::RenderWindow& window)
{
	win->setView(*states->getUiView());
	win->draw(overlay);
	win->draw(winBox);
	restart.render(*win);
	back.render(*win);
	win->draw(text);
}