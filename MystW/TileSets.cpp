#include "TileSets.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm> // Bắt buộc

// --- Hằng số cờ lật/xoay của Tiled ---
const unsigned int FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned int FLIPPED_VERTICALLY_FLAG = 0x40000000;
const unsigned int FLIPPED_DIAGONALLY_FLAG = 0x20000000;

Tileset::Tileset() : m_tileSize(0), m_width(0), m_height(0) {}

void Tileset::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = &m_texture;
    target.draw(m_vertices, states);
}

bool Tileset::load(const std::string& tilesetPath, const std::string& mapPath, unsigned int tileSize, sf::Vector2f windowSize)
{
    // --- PHẦN CODE GỐC CỦA BẠN (GIỮ NGUYÊN) ---
    m_tileSize = tileSize;
    m_texture.setSmooth(false);
    if (!m_texture.loadFromFile(tilesetPath)) { /* ... */ return false; }
    std::ifstream file(mapPath);
    if (!file.is_open()) { /* ... */ return false; }
    m_mapData.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            try { row.push_back(std::stoi(cell)); }
            catch (...) { row.push_back(-1); } // Sửa thành -1 để nhất quán
        }
        m_mapData.push_back(row);
    }
    file.close();
    if (m_mapData.empty()) { /* ... */ return false; }

    m_height = m_mapData.size();
    m_width = m_mapData[0].size();

    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(m_width * m_height * 4);

    unsigned int quadIndex = 0;

    for (unsigned int y = 0; y < m_height; ++y) {
        for (unsigned int x = 0; x < m_width; ++x) {
            int rawGid = m_mapData[y][x];

            if (rawGid == -1) { // Chỉ bỏ qua các ô trống tường minh
                continue;
            }

            // --- BẮT ĐẦU SỬA ĐỔI LOGIC ---
            unsigned int tileGid = static_cast<unsigned int>(rawGid);
            bool flippedH = (tileGid & FLIPPED_HORIZONTALLY_FLAG);
            bool flippedV = (tileGid & FLIPPED_VERTICALLY_FLAG);
            bool flippedD = (tileGid & FLIPPED_DIAGONALLY_FLAG);

            // "Dọn dẹp" các cờ để lấy lại ID gốc
            tileGid &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

            // QUAN TRỌNG: Gán lại ID đã dọn dẹp cho tileNumber
            // Bước này đảm bảo tileNumber luôn là số dương để tính toán
            int tileNumber = tileGid;

            // --- PHẦN CÒN LẠI GIỮ NGUYÊN HOÀN TOÀN NHƯ CODE GỐC CỦA BẠN ---
            int tu = tileNumber % (m_texture.getSize().x / m_tileSize);
            int tv = tileNumber / (m_texture.getSize().x / m_tileSize);

            sf::Vertex* quad = &m_vertices[quadIndex * 4];

            quad[0].position = sf::Vector2f(x * m_tileSize, y * m_tileSize);
            quad[1].position = sf::Vector2f((x + 1) * m_tileSize, y * m_tileSize);
            quad[2].position = sf::Vector2f((x + 1) * m_tileSize, (y + 1) * m_tileSize);
            quad[3].position = sf::Vector2f(x * m_tileSize, (y + 1) * m_tileSize);

            // Áp dụng logic lật xoay lên tọa độ texture
            sf::Vector2f topLeft(tu * m_tileSize, tv * m_tileSize);
            sf::Vector2f topRight((tu + 1) * m_tileSize, tv * m_tileSize);
            sf::Vector2f bottomLeft(tu * m_tileSize, (tv + 1) * m_tileSize);
            sf::Vector2f bottomRight((tu + 1) * m_tileSize, (tv + 1) * m_tileSize);

            quad[0].texCoords = topLeft;
            quad[1].texCoords = topRight;
            quad[2].texCoords = bottomRight;
            quad[3].texCoords = bottomLeft;

            if (flippedD) { // Logic xoay/lật phức tạp hơn một chút
                if (flippedH && flippedV) {
                    quad[1].texCoords = topLeft; quad[2].texCoords = topRight; quad[3].texCoords = bottomRight; quad[0].texCoords = bottomLeft;
                }
                else if (flippedH) {
                    quad[0].texCoords = topRight; quad[1].texCoords = bottomRight; quad[2].texCoords = bottomLeft; quad[3].texCoords = topLeft;
                }
                else if (flippedV) {
                    quad[0].texCoords = bottomLeft; quad[1].texCoords = topLeft; quad[2].texCoords = topRight; quad[3].texCoords = bottomRight;
                }
                else {
                    quad[0].texCoords = topRight; quad[1].texCoords = topLeft; quad[2].texCoords = bottomLeft; quad[3].texCoords = bottomRight;
                }
            }
            else { // Chỉ lật ngang hoặc dọc
                if (flippedH) {
                    std::swap(quad[0].texCoords, quad[1].texCoords);
                    std::swap(quad[2].texCoords, quad[3].texCoords);
                }
                if (flippedV) {
                    std::swap(quad[0].texCoords, quad[3].texCoords);
                    std::swap(quad[1].texCoords, quad[2].texCoords);
                }
            }

            quadIndex++;
        }
    }

    m_vertices.resize(quadIndex * 4);

    return true;
}