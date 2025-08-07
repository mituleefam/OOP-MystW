#include "MenuState.hpp"
#include "GameState.hpp"
#include "TrophyPage.hpp"
#include "SoundPage.hpp"
#include <iostream>

MenuState::MenuState(StateManager *sm) : states(sm) {
	win = states->getWindow();

	if (!trophyTex.loadFromFile("image/icon/trophy_icon.png"))
		std::cout << "Cannot load trophy icon \n";

	if (!soundTex.loadFromFile("image/icon/sound_icon.png"))
		std::cout << "Cannot load sound icon \n";

	if (!font1.loadFromFile("font/ManufacturingConsent-Regular.ttf"))
		std::cout << "Cannot load first font \n";

	if (!font2.loadFromFile("font/Merriweather-VariableFont_opsz,wdth,wght.ttf"))
		std::cout << "Cannot load second font \n";

	text.setFont(font1);

	menuB.loadFromFile("image/Menu/backgroundMenu.jpg");
	menuBackground.setTexture(menuB);
	float scaleX = 1920.0f / static_cast<float>(menuB.getSize().x);
	float scaleY = 1080.0f / static_cast<float>(menuB.getSize().y);

	menuBackground.setScale({ scaleX,scaleY });

	//Play Button
	play.setShape(720, 160, 400, 160);
	play.setText(font1, "Play");

	//Exit Button
	exit.setShape(720, 540, 400, 160);
	exit.setText(font1, "Exit");

	//Trophy icon
	trophy.setIcon(trophyTex, 720, 360, 128.0f);
	trophy.setShape(680, 350, 200, 160);

	//Sound icon
	sound.setIcon(soundTex, 1080, 360, 128.0f);
	sound.setShape(1020, 350, 200, 160);

}

void MenuState::handleEvent(sf::Event& event)
{
	if (play.isClicked(*win, event))
	{
		std::cout << "Game state is clicked\n";
		states->pushState(std::make_unique<GameState>(states, win));
	}

	if (exit.isClicked(*win, event))
		win->close();

	if (sound.isClicked(*win, event))
	{
		std::cout << "Sound button is clicked \n";
		// states->pushState(std::make_unique<SoundPage>(states, win));
		states->pushState(std::make_unique<SoundPage>(states));
	}

	if (trophy.isClicked(*win, event))
	{
		std::cout << "Trophy button is clicked \n";
		// states->pushState(std::make_unique<TrophyPage>(states, win));
		states->pushState(std::make_unique<TrophyPage>(states));
	}

}

void MenuState::update(float delta)
{
	play.applyHoverEffect(*win);
	exit.applyHoverEffect(*win);
	trophy.applyHoverEffect(*win);
	sound.applyHoverEffect(*win);
}

void MenuState::render(sf::RenderWindow& window)
{
	window.setView(*states->getUiView());

	window.draw(menuBackground);
	play.render(window);
	exit.render(window);
	sound.render(window);
	trophy.render(window);
}

