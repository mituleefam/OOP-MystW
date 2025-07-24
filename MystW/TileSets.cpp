#include "TileSets.h"
#include <fstream>
#include <sstream>
#include <iostream>
Tileset::Tileset() : m_tileSize(0), m_width(0), m_height(0) {}

bool Tileset::load(const std::string& tilesetPath, const std::string& mapPath, unsigned int tileSize, sf::Vector2f windowSize)
{
    m_tileSize = tileSize;

    //Load tileset texture
    if (!m_texture.loadFromFile(tilesetPath)) {
        std::cout << "Error: Cannot load file tileset: " << tilesetPath << std::endl;
        return false;
    }

    //Read map CSV
    std::ifstream file(mapPath);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file map: " << mapPath << std::endl;
        return false;
    }

    m_mapData.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(std::stoi(cell));
        }
        m_mapData.push_back(row);
    }
    file.close();

    if (m_mapData.empty()) {
        std::cout << "Error: Du lieu map rong." << std::endl;
        return false;
    }

    //Map size
    m_height = m_mapData.size();
    m_width = m_mapData[0].size(); // Số cột (tile theo ngang)

    const unsigned int maxVisibleX = 25;
    const unsigned int maxVisibleY = 19;



    //Prepare vertices
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(m_width * m_height * 4);

    int xOffset = 0;
    int yOffset = m_height > maxVisibleY ? m_height - maxVisibleY : 0;

    bool found = false;

    for (unsigned int y = 0; y < m_height && !found; ++y) {
        for (unsigned int x = 0; x < m_width && !found; ++x) {
            if (m_mapData[y][x] >= 0) {
                xOffset = x;
                yOffset = y;
                found = true;
            }
        }
    }
    std::cout << "Offset: (" << xOffset << ", " << yOffset << ")\n";

    // Tính số tile hiển thị
    unsigned int visibleTileWidth = std::min(xOffset + maxVisibleX, m_width) - xOffset;
    unsigned int visibleTileHeight = std::min(yOffset + maxVisibleY, m_height) - yOffset;

    // Tính kích thước thật của tilemap đang vẽ
    float mapPixelWidth = visibleTileWidth * m_tileSize;
    float mapPixelHeight = visibleTileHeight * m_tileSize;

    // Scale để vừa khớp trong khung 
    float scaleX = windowSize.x / mapPixelWidth;
    float scaleY = windowSize.y / mapPixelHeight;


    // Dùng scale nhỏ hơn để giữ đúng tỉ lệ (không bị méo)
    float finalScale = std::min(scaleX, scaleY);
    unsigned int quadIndex = 0;

    for (unsigned int y = yOffset; y < std::min(yOffset + maxVisibleY, m_height); ++y) {
        for (unsigned int x = xOffset; x < std::min(xOffset + maxVisibleX, m_width); ++x) {
            int tileNumber = m_mapData[y][x];
            if (tileNumber < 0) continue;

            int tu = tileNumber % (m_texture.getSize().x / m_tileSize);
            int tv = tileNumber / (m_texture.getSize().x / m_tileSize);

            sf::Vertex* quad = &m_vertices[quadIndex * 4];


            quad[0].position = sf::Vector2f((x - xOffset) * m_tileSize * finalScale, (y - yOffset) * m_tileSize * finalScale);
            quad[1].position = sf::Vector2f((x - xOffset + 1) * m_tileSize * finalScale, (y - yOffset) * m_tileSize * finalScale);
            quad[2].position = sf::Vector2f((x - xOffset + 1) * m_tileSize * finalScale, (y - yOffset + 1) * m_tileSize * finalScale);
            quad[3].position = sf::Vector2f((x - xOffset) * m_tileSize * finalScale, (y - yOffset + 1) * m_tileSize * finalScale);


            quad[0].texCoords = sf::Vector2f(tu * m_tileSize, tv * m_tileSize);
            quad[1].texCoords = sf::Vector2f((tu + 1) * m_tileSize, tv * m_tileSize);
            quad[2].texCoords = sf::Vector2f((tu + 1) * m_tileSize, (tv + 1) * m_tileSize);
            quad[3].texCoords = sf::Vector2f(tu * m_tileSize, (tv + 1) * m_tileSize);

            quadIndex++;
        }
    }

    // Resize lại đúng với số lượng quad thực sự dùng
    m_vertices.resize(quadIndex * 4);


    // In thông tin tile đầu tiên được vẽ (để debug)
    for (unsigned int y = yOffset; y < std::min(yOffset + maxVisibleY, m_height); ++y) {
        for (unsigned int x = xOffset; x < std::min(xOffset + maxVisibleX, m_width); ++x) {
            int tileNumber = m_mapData[y][x];
            if (tileNumber < 0) continue;

            std::cout << "First valid tile at (" << x << "," << y << ") = " << tileNumber << "\n";
            sf::Vertex* quad = &m_vertices[0]; // lấy cái đầu tiên (quadIndex == 0)
            for (int k = 0; k < 4; ++k)
                std::cout << "(" << quad[k].position.x << "," << quad[k].position.y << ") ";
            std::cout << "\n";
            goto done;
        }
    }

done:;

    return true;
}


void Tileset::draw(sf::RenderTarget& target, sf::RenderStates states) const { 
    states.texture = &m_texture;
    target.draw(m_vertices, states);
}
