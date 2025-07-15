#include "PausePage.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"

PausePage::PausePage(StateManager* sm, sf::RenderWindow* window) : states(sm), win(window)
{
	resTex.loadFromFile("image/icon/resume_icon.png");
	backTex.loadFromFile("image/icon/home_icon.png");
	sf::Vector2f center(1920.f / 2.f, 1080.f / 2.f);

	back.setIcon(backTex, center.x - 300, center.y - 90, 200.0f);
	back.setShape(center.x - 300, center.y - 90, 200, 200);

	resume.setIcon(resTex, center.x, center.y - 90, 200.0f);
	resume.setShape(center.x, center.y - 90, 200, 200);

	overlay.setSize(sf::Vector2f(1920.f, 1080.f));
	overlay.setFillColor(sf::Color(0, 0, 0, 100));

	pauseBox.setSize(sf::Vector2f(960.f, 540.f));
	pauseBox.setFillColor(sf::Color(200, 200, 200, 240));
	pauseBox.setOutlineColor(sf::Color::Black);
	pauseBox.setOutlineThickness(2);
	pauseBox.setOrigin(480.f, 270.f);
	pauseBox.setPosition(center);

}

void PausePage::handleEvent(sf::Event& event)
{
	if (resume.isClicked(*win, event))
		isResumeClicked = true;

	if (back.isClicked(*win, event))
		isHomeClicked = true;
}

void PausePage::update(float delta)
{
	resume.applyHoverEffect(*win);
	back.applyHoverEffect(*win);
	if (isResumeClicked)
	{
		isResumeClicked = false;
		states->popState();
		return;
	}

	if (isHomeClicked)
	{
		states->pushState(std::make_unique<MenuState>(states, win));
		isHomeClicked = false;
		return;
	}
}

void PausePage::render(sf::RenderWindow& window)
{
	win->draw(overlay);
	win->draw(pauseBox);
	resume.render(*win);
	back.render(*win);
}