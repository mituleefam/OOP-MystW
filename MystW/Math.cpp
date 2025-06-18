#include "Math.h"
#include <SFML/Graphics.hpp>
#include <cmath>

void normalizeVector(sf::Vector2f& vector)
{
	float length = sqrt(vector.x * vector.x + vector.y * vector.y);
	if (length != 0)
	{
		vector.x /= length;
		vector.y /= length;
	}
}
float magnitude(const sf::Vector2f& vector)
{
	return sqrt(vector.x * vector.x + vector.y * vector.y);
}