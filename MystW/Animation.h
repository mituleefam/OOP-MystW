#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

struct Animation {
    // A way to identify the type of animation
    enum class AnimationType {
        SpriteSheet, // Uses one texture sheet and IntRects
        TextureList  // Uses a list of separate textures
    };
    /*std::vector<std::shared_ptr<sf::Texture>> frames;*/
    AnimationType type = AnimationType::SpriteSheet; // Default to SpriteSheet
    float frameDuration = 0.1f;
    bool loop = true;
    // --- SpriteSheet members (for Enemy) ---
    std::shared_ptr<sf::Texture> sheet;
    std::vector<sf::IntRect> frames;

    // --- TextureList members (for Bosses) ---
    std::vector<std::shared_ptr<sf::Texture>> textureFrames;
};