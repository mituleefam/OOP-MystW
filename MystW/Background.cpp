#include "Background.hpp"
#include <filesystem>
#include <iostream>

BackgroundLayer::BackgroundLayer(float factor) : parallaxFactor(factor), textureWidth(0), windowRef(nullptr) {}

// HÀM UPDATE MỚI THAY THẾ CHO updateMoveLeft/Right
void BackgroundLayer::update(sf::Vector2f cameraPosition) {
    if (!windowRef) return;

    // 1. Tính toán vị trí góc trên bên trái của camera
    sf::Vector2f viewTopLeft = cameraPosition - (windowRef->getView().getSize() / 2.f);

    // 2. Tính toán độ lệch khi cuộn texture (lấy phần dư sau khi chia cho chiều rộng texture)
    float textureOffsetX = std::fmod(viewTopLeft.x * parallaxFactor, static_cast<float>(textureWidth));
    if (textureOffsetX < 0) {
        textureOffsetX += textureWidth;
    }
    // 3. Lấy kích thước view của cửa sổ
    sf::Vector2f viewSize = windowRef->getView().getSize();

    // 4. Tính toán chiều rộng của vùng texture cần vẽ
    //    Đảm bảo nó bao phủ toàn bộ chiều rộng view
    float textureDrawWidth = viewSize.x / sprite.getScale().x;

    // 5. Xác định vị trí bắt đầu (left) của texture rect
    float textureStart = textureOffsetX;

    // Nếu phần bắt đầu vượt quá chiều rộng texture, cần điều chỉnh
    if (textureStart + textureDrawWidth > textureWidth) {
        textureStart -= textureWidth;
    }

    // 6. Tạo texture rect
    sprite.setTextureRect(sf::IntRect(
        static_cast<int>(textureStart),
        0,
        static_cast<int>(textureDrawWidth),
        texture.getSize().y
    ));
}

void BackgroundLayer::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void BackgroundLayer::load(const std::string& path, sf::RenderWindow& window)
{
    if (texture.loadFromFile(path))
    {
        windowRef = &window;
        textureWidth = texture.getSize().x;
        sprite.setTexture(texture);
		texture.setRepeated(true); // Cho phép lặp lại texture

        // Đặt vị trí của sprite CỐ ĐỊNH ở góc (0,0) của màn hình.
        sprite.setPosition(0.f, 0.f);

        // Scale sprite để chiều cao của nó vừa với chiều cao cửa sổ
        float scale = 1080.0f / static_cast<float>(texture.getSize().y);
        sprite.setScale(scale, scale);
    }
}

void ManageLayer::loadStage(const std::string& folderPath, sf::RenderWindow& window)
{
    layers.clear();
    // Hệ số parallax sẽ giảm dần cho các lớp phía sau
    float baseFactor = 0.7f;
    float factorStep = 0.2f;
    int layerIndex = 0;
    std::list<std::string> imagePath;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
    {
        if (!entry.is_regular_file())
            continue;
        std::string path = entry.path().string();
        if (path.substr(path.size() - 4) == ".png" || path.substr(path.size() - 4) == ".jpg")
        {
            std::cout << "Take path successfully \n";
            imagePath.push_back(path);
        }
    }
    imagePath.sort();

    for (const auto& path : imagePath)
    {
        float currentFactor = baseFactor - (layerIndex * factorStep);
        if (currentFactor < 0.1f) currentFactor = 0.1f; // Giới hạn hệ số tối thiểu

        layers.emplace_back(currentFactor);
        layers.back().load(path, window);
        layerIndex++;
    }
}

void ManageLayer::update(sf::Vector2f cameraPosition) {
    for (auto& layer : layers) {
        layer.update(cameraPosition);
    }
}

void ManageLayer::draw(sf::RenderWindow& window)
{
    for (auto& layer : layers)
        layer.draw(window);
}