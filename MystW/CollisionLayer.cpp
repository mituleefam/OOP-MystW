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
    // Chuyển đổi tọa độ pixel sang tọa độ tile
    int tileX = x / m_tileSize;
    int tileY = y / m_tileSize;

    // Kiểm tra xem tọa độ có nằm trong bản đồ không
    if (tileX < 0 || tileX >= m_width || tileY < 0 || tileY >= m_height) {
        return true; // Coi như ngoài bản đồ là vật cản
    }

    // Kiểm tra giá trị trong mảng dữ liệu collision
    // Giả sử '1' là vật cản
    return m_collisionData[tileY][tileX] == 1;
}

unsigned int CollisionLayer::getTileSize() const {
    return m_tileSize;
}