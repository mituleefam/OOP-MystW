#pragma once
#include <SFML/Graphics.hpp>

class Button
{
private:
	sf::RectangleShape shape;
	sf::Text text;
	sf::Font font;
	sf::Sprite icon;
	bool hasIcon = false;
	std::string outlineColor = "2e2e2e", backgroundColor = "aaaaaa", hoverColor = "c0a000";

public:
	Button(){}
	void setText(const sf::Font& font, const std::string& name);
	void setShape(float x, float y, float width, float height);
	void setIcon(const sf::Texture& texture, float x, float y, float scale);
	void render(sf::RenderWindow& window);
	bool isHovered(sf::RenderWindow& window);
	void applyHoverEffect(sf::RenderWindow& window);
	bool isClicked(sf::RenderWindow& window, sf::Event& event);
	sf::Color hexToColor(std::string& color);
};