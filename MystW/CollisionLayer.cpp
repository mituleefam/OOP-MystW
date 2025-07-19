#include "CollisionLayer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

CollisionLayer::CollisionLayer() : m_tileSize(0), m_width(0), m_height(0) {}

bool CollisionLayer::load(const std::string& collisionPath, unsigned int tileSize) {
    m_tileSize = tileSize;

    std::ifstream file(collisionPath);
    if (!file.is_open()) {
        std::cerr << "Error: Khong the mo file collision: " << collisionPath << std::endl;
        return false;
    }

    m_collisionData.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(std::stoi(cell));
        }
        m_collisionData.push_back(row);
    }
    file.close();

    if (m_collisionData.empty()) {
        std::cerr << "Error: Du lieu collision rong." << std::endl;
        return false;
    }

    m_height = m_collisionData.size();
    m_width = m_collisionData[0].size();

    return true;
}

bool CollisionLayer::isCollidable(int x, int y) {
    int tileX = static_cast<int>(std::floor(x / static_cast<float>(m_tileSize)));
    int tileY = static_cast<int>(std::floor(y / static_cast<float>(m_tileSize)));

    if (tileX < 0 || tileX >= m_width || tileY < 0 || tileY >= m_height)
        return true; // ngoài bản đồ = vật cản

    return m_collisionData[tileY][tileX] == 192;
}


unsigned int CollisionLayer::getTileSize() const {
    return m_tileSize;
}