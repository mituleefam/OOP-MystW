#include "Background.hpp"
#include <filesystem>
#include <iostream>

// BackgroundLayer::BackgroundLayer(float speed) : scrollSpeed(speed) {}
BackgroundLayer::BackgroundLayer(float factor) : parallaxFactor(factor), textureWidth(0), windowRef(nullptr) {}

//void BackgroundLayer::updateMoveRight(float delta) {
//    //Scrolling
//    sprite1.move(-scrollSpeed * delta, 0.0f);
//    sprite2.move(-scrollSpeed * delta, 0.0f);
//    sprite3.move(-scrollSpeed * delta, 0.0f);
//
//    float width = sprite1.getGlobalBounds().width;
//    float totalWidth = width * 3.0f;
//
//    // Move forward a distance equal to the half of total width when background go out the screen
//    if (sprite1.getPosition().x <= -width)
//        sprite1.move(totalWidth, 0.0f);
//    if (sprite2.getPosition().x <= -width)
//        sprite2.move(totalWidth, 0.0f);
//    if (sprite3.getPosition().x <= -width)
//        sprite3.move(totalWidth, 0.0f);
//}
//
//void BackgroundLayer::updateMoveLeft(float delta) {
//    //Scrolling
//    sprite1.move(scrollSpeed * delta, 0.0f);
//    sprite2.move(scrollSpeed * delta, 0.0f);
//    sprite3.move(scrollSpeed * delta, 0.0f);
//
//    float width = sprite1.getGlobalBounds().width;
//    float totalWidth = width * 3.0f;
//
//    // Move forward a distance equal to the half of total width when background go out the screen
//    if (sprite1.getPosition().x >= width)
//        sprite1.move(-totalWidth, 0.0f);
//    if (sprite2.getPosition().x >= width)
//        sprite2.move(-totalWidth, 0.0f);
//    if (sprite3.getPosition().x >= width)
//        sprite3.move(-totalWidth, 0.0f);
//
//}
// HÀM UPDATE MỚI THAY THẾ CHO updateMoveLeft/Right
void BackgroundLayer::update(sf::Vector2f cameraPosition) {
    if (!windowRef) return;

    // 1. Tính toán vị trí góc trên bên trái của camera
    sf::Vector2f viewTopLeft = cameraPosition - (windowRef->getView().getSize() / 2.f);

    //// 2. Tính toán vị trí cuộn của texture
    ////    Dựa trên vị trí camera và hệ số parallax
    //float textureScrollX = std::fmod(viewTopLeft.x * parallaxFactor, static_cast<float>(textureWidth));
    //if (textureScrollX < 0) {
    //    textureScrollX += textureWidth;
    //}

    //// 3. Đặt vùng texture cần vẽ. Chiều rộng lấy theo kích thước view của cửa sổ
    ////    Lưu ý: cần chuyển đổi kích thước view (có thể đã scale) sang kích thước texture
    //float viewWidthInTextureCoords = windowRef->getView().getSize().x / sprite.getScale().x;
    //sprite.setTextureRect(sf::IntRect(static_cast<int>(textureScrollX), 0, static_cast<int>(viewWidthInTextureCoords), texture.getSize().y));
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
    window.draw(sprite);//1);
    //window.draw(sprite2);
    //window.draw(sprite3);
}

void BackgroundLayer::load(const std::string& path, sf::RenderWindow& window)
{
//    if (texture.loadFromFile(path))
//    {
//        sf::Vector2f windowSize = window.getView().getSize();
//        sf::Vector2u textureSize = texture.getSize();
//        float scaleX = windowSize.x / static_cast<float>(textureSize.x);
//        float scaleY = windowSize.y / static_cast<float>(textureSize.y);
//        std::cout << "Loaded: " << path << std::endl;
//
//        sprite1.setTexture(texture);
//        sprite2.setTexture(texture);
//        //sprite3.setTexture(texture);
//
//        sprite1.setScale(scaleX, scaleY);
//        sprite2.setScale(scaleX, scaleY);
//        //sprite3.setScale(scaleX, scaleY);
//
//        float scaleWidth = sprite1.getGlobalBounds().width;
//
//        sprite2.setPosition(scaleWidth, 0.0f);
//        //sprite3.setPosition(scaleWidth * 2.0f, 0.0f);
//
//    }
//    else
//        std::cout << "Cannot load file \n";
    if (texture.loadFromFile(path))
    {
        windowRef = &window;
        textureWidth = texture.getSize().x;
        sprite.setTexture(texture);
		texture.setRepeated(true); // Cho phép lặp lại texture

        // Đặt vị trí của sprite CỐ ĐỊNH ở góc (0,0) của màn hình.
        sprite.setPosition(0.f, 0.f);

        // Scale sprite để chiều cao của nó vừa với chiều cao cửa sổ
        //sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
        //float scale = windowSize.y / static_cast<float>(texture.getSize().y);
        float scale = 1080.0f / static_cast<float>(texture.getSize().y);
        sprite.setScale(scale, scale);
    }
}

void ManageLayer::loadStage(const std::string& folderPath, sf::RenderWindow& window)
{
    layers.clear();
    //float baseSpeed = 50.0f;
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

        //layers.emplace_back(baseSpeed + (layerIndex * 20.0f));
        layers.emplace_back(currentFactor);
        layers.back().load(path, window);
        layerIndex++;
    }
}

//void ManageLayer::update(float delta, char pressButton)
//{
//    for (auto& layer : layers)
//    {
//        if (pressButton == 'A')
//            layer.updateMoveLeft(delta);
//        else if (pressButton == 'D')
//            layer.updateMoveRight(delta);
//    }
//}
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