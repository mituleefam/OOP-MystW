#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

struct Animation {
    /*std::vector<std::shared_ptr<sf::Texture>> frames;*/
    std::shared_ptr<sf::Texture> sheet;
    std::vector<sf::IntRect> frames;
    float frameDuration = 0.1f;
    bool loop = true;
};