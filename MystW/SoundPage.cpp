#include "SoundPage.hpp"
#include <iostream>

//SoundPage::SoundPage(StateManager* sm, sf::RenderWindow* window) : states(sm), win(window)
SoundPage::SoundPage(StateManager* sm) : states(sm)
{
	win = states->getWindow();

	sf::Vector2f viewSize = win->getView().getSize();

	auto scaleX = viewSize.x / 800.f;
	auto scaleY = viewSize.y / 600.f;

	if (!volOn.loadFromFile("image/icon/volumeOn_icon.png"))
		std::cout << "Cannot load volume on \n";
	if (!volOff.loadFromFile("image/icon/volumeOff_icon.png"))
		std::cout << "Cannot load volume off \n";

	volumeOn.setTexture(volOn);
	volumeOff.setTexture(volOff);

	volumeOn.setPosition(200.f * scaleX, 290.f * scaleY);
	volumeOff.setPosition(200.f * scaleX, 290.f * scaleY);

	volumeOn.setScale(1.5f * scaleX, 1.5f * scaleY);
	volumeOff.setScale(1.5f * scaleX, 1.5f * scaleY);

	soundBox.setSize({ 400.f * scaleX, 300.f * scaleY });
	soundBox.setFillColor(sf::Color(200, 230, 201, 255));
	soundBox.setOutlineColor(sf::Color::Black);
	soundBox.setOutlineThickness(2);
	soundBox.setOrigin(200.f * scaleX, 150.f * scaleY);
	soundBox.setPosition(400.f * scaleX, 300.f * scaleY);

	slideBar.setSize({ 300.f * scaleX, 8.f * scaleY });
	slideBar.setPosition(250.f * scaleX, 300.f * scaleY);
	slideBar.setFillColor(sf::Color::White);

	track.setSize({ (curVol / 100.f) * slideBar.getSize().x , 8.f * scaleY });
	track.setPosition(250.f * scaleX, 300.f * scaleY);
	track.setFillColor(sf::Color(129, 212, 250, 255));

	sliderKnob.setRadius(12.f * scaleX);
	sliderKnob.setFillColor(sf::Color(255, 255, 255));
	sliderKnob.setOutlineColor(sf::Color(176, 190, 197));
	sliderKnob.setOrigin(12.f * scaleX, 12.f * scaleY);
	sliderKnob.setPosition(slideBar.getPosition().x + (curVol / 100.f) * slideBar.getSize().x,304.f * scaleY);
}


void SoundPage::handleEvent(sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		states->popState();
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
	{
		sf::Vector2f mousePos = win->mapPixelToCoords(sf::Mouse::getPosition(*win));
		if (sliderKnob.getGlobalBounds().contains(mousePos) || slideBar.getGlobalBounds().contains(mousePos))
		{
			isDragging = true;
			sliderKnob.setFillColor(sf::Color(245, 245, 245));
		}
	}
	if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
	{
		isDragging = false;
		sliderKnob.setFillColor(sf::Color(255, 255, 255));
	}
}

void SoundPage::update(float delta) {
	if (isDragging)
	{
		auto scaleY = 1080.0f / 600.f;
		sf::Vector2f mouse = win->mapPixelToCoords(sf::Mouse::getPosition(*win));
		float mouseX = static_cast<float>(mouse.x);
		float barX = slideBar.getPosition().x;
		float barWidth = slideBar.getSize().x;
		mouseX = std::max(barX, std::min(mouseX, barX + barWidth));
		curVol = ((mouseX - barX) / barWidth) * 100.0f;
		sliderKnob.setPosition(barX + (curVol / 100.0f) * barWidth, sliderKnob.getPosition().y);
		track.setSize({ curVol / 100.0f * slideBar.getSize().x , 8.f*scaleY });
	}
}

void SoundPage::render(sf::RenderWindow& window)
{
	window.setView(*states->getUiView());

	window.draw(soundBox);
	window.draw(slideBar);
	window.draw(track);
	window.draw(sliderKnob);
	window.draw(curVol > 0 ? volumeOn : volumeOff);
	sf::Font font;
	font.loadFromFile("font/Roboto.ttf");
	float scaleX = 1920.0f / 800.0f;
	float scaleY = 1080.0f / 600.0f;
	sf::Text final;
	final.setFont(font);
	final.setCharacterSize(70);
	final.setFillColor(sf::Color(255, 215, 0));
	final.setString("Press Esc to out");
	final.setPosition(260 * scaleX, (150 + 3 * 60 + 60) * scaleY);
	window.draw(final);
}
