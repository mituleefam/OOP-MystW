#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Tileset : public sf::Drawable { 
public:
    Tileset();
    bool load(const std::string& tilesetPath, const std::string& mapPath, unsigned int tileSize, sf::Vector2f windowSize);
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Texture m_texture;
    sf::VertexArray m_vertices;
    std::vector<std::vector<int>> m_mapData;
    unsigned int m_tileSize;
    unsigned int m_width;
    unsigned int m_height;
};

