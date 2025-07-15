#include "TrophyPage.hpp"
#include "ScoreManager.hpp"
TrophyPage::TrophyPage(StateManager* sm, sf::RenderWindow* window) : states(sm), win(window)
{
	float scaleX = 1920.0f / 800.0f;
	float scaleY = 1080.0f / 600.0f;
	trophyBox.setSize(sf::Vector2f(1000, 600));
	trophyBox.setFillColor(sf::Color(250, 243, 224, 255));
	trophyBox.setOutlineColor(sf::Color(255, 224, 130, 255));
	trophyBox.setOutlineThickness(2);
	trophyBox.setOrigin(500, 300);
	trophyBox.setPosition(400*scaleX, 300*scaleY);

	font.loadFromFile("font/Roboto.ttf");
	auto scores = ScoreManager::getTopScore();
	sf::Text header;
	header.setFont(font);
	header.setCharacterSize(70);
	header.setFillColor(sf::Color(255, 215, 0));
	header.setString("TOP 3 HIGHEST SCORES: ");
	header.setPosition(250 * scaleX, 150 * scaleY);
	scoreText.push_back(header);
	for (size_t i = 0; i < scores.size(); ++i)
	{
		sf::Text text;
		text.setFont(font);
		text.setCharacterSize(70);
		switch (i)
		{
		case 0:
			text.setFillColor(sf::Color(255, 215, 0));
			break;
		case 1:
			text.setFillColor(sf::Color(192, 192, 192, 255));
			break;
		case 2:
			text.setFillColor(sf::Color(205, 127, 50, 255));
			break;
		}

		text.setString("Top " + std::to_string(i + 1) + ": "+std::to_string(scores[i]));
		text.setPosition(300*scaleX, (150 + i * 60 + 60)*scaleY);
		scoreText.push_back(text);
	}
	sf::Text final;
	final.setFont(font);
	final.setCharacterSize(70);
	final.setFillColor(sf::Color(255, 215, 0));
	final.setString("Press Esc to out");
	final.setPosition(260 * scaleX, (150 + 3 * 60 + 60) * scaleY);
	scoreText.push_back(final);
}

void TrophyPage::handleEvent(sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		states->popState();
}

void TrophyPage::update(float delta){}

void TrophyPage::render(sf::RenderWindow& window)
{
	window.draw(trophyBox);
	for (auto text : scoreText)
		window.draw(text);
}