#include "Button.hpp"
#include <sstream>
sf::Color Button::hexToColor(std::string& color)
{
	unsigned int r = 0, b = 0, g = 0, a = 255;
	std::stringstream ss;
	if (color.length() == 6 || color.length() == 8)
	{
		ss << std::hex << color.substr(0, 2);
		ss >> r;
		ss.clear();

		ss << std::hex << color.substr(2, 2);
		ss >> g;
		ss.clear();

		ss << std::hex << color.substr(4, 2);
		ss >> b;
		ss.clear();

		if (color.length() == 8)
		{
			ss << std::hex << color.substr(6, 2);
			ss >> a;
			ss.clear();
		}
	}
	return sf::Color(r, g, b, a);
}
void  Button::setText(const sf::Font& font, const std::string& name)
{
	text.setFont(font);
	text.setString(name);
	text.setCharacterSize(70);

	//Center the text
	sf::FloatRect textBound = text.getLocalBounds();
	text.setOrigin(textBound.width / 2, textBound.height / 2);
	text.setPosition(shape.getPosition().x + shape.getSize().x / 2, shape.getPosition().y - 10 + shape.getSize().y / 2);
}

void Button::setShape(float x, float y, float width, float height, std::string outlineCol, std::string backgroundCol)
{
	outlineColor = outlineCol;
	backgroundColor = backgroundCol;

	shape.setPosition(x, y);
	shape.setSize({ width,height });
	shape.setOutlineColor(hexToColor(outlineColor));
	shape.setFillColor(hexToColor(backgroundColor));
	shape.setOutlineThickness(1.5f);
}

void Button::setIcon(const sf::Texture& texture, float x, float y, float scale)
{
	hasIcon = true;
	icon.setTexture(texture);
	icon.setPosition(x, y);
	icon.setScale(scale / static_cast<float>(texture.getSize().x), scale / static_cast<float>(texture.getSize().y));
}

void Button::render(sf::RenderWindow& window)
{
	if (hasIcon)
	{
		window.draw(shape);
		window.draw(icon);
	}
	else
	{
		window.draw(shape);
		window.draw(text);
	}
}

bool Button::isHovered(sf::RenderWindow& window)
{
	sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
	sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos); // transfer from pixel to world coord
	return shape.getGlobalBounds().contains(worldPos);

}

void Button::applyHoverEffect(sf::RenderWindow& window, std::string hoverCol, std::string backgroundCol)
{
	if (isHovered(window))
	{
		hoverColor = hoverCol;
		shape.setFillColor(hexToColor(hoverColor));
		shape.setScale(1.1f, 1.1f);
	}
	else
	{
		backgroundColor = backgroundCol;
		shape.setFillColor(hexToColor(backgroundColor));
		shape.setScale(1.0f, 1.0f);
	}
}

bool Button::isClicked(sf::RenderWindow& window, sf::Event& event)
{
	return (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && isHovered(window));
}