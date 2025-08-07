#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "TileSets.h"

class CollisionLayer {
public:
    // Hàm khởi tạo
    CollisionLayer();

    // Tải dữ liệu va chạm từ file CSV
    bool load(const std::string& collisionPath, unsigned int tileSize);

    // Kiểm tra xem một tile tại tọa độ (x, y) có phải là vật cản không
    bool isCollidable(int x, int y) const;

    // Lấy kích thước của một tile
    unsigned int getTileSize() const;

private:
    std::vector<std::vector<int>> m_collisionData; // Dữ liệu va chạm
    unsigned int m_tileSize;        // Kích thước của tile
    unsigned int m_width;           // Chiều rộng của map (số lượng tile)
    unsigned int m_height;          // Chiều cao của map (số lượng tile)
};
