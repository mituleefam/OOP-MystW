#pragma once
#include "Scene.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class MenuScene : public Scene {
private:
    int selectedItemIndex;
    sf::Font font;
    std::vector<sf::Text> menuItems;

public:
    MenuScene(SceneManager* manager); // constructor truy?n SceneManager

    // Override các ph??ng th?c t? Scene
    void handleEvent(sf::Event event) override;
    void update(sf::Time delta) override;
    void render(sf::RenderWindow& window) override;

    // Các hàm gi? nguyên tính n?ng c? nh?ng ??i tên
    void moveUp();      // T??ng ???ng MoveUp
    void moveDown();    // T??ng ???ng MoveDown
    int getSelectedIndex() const; // T??ng ???ng GetSelectedIndex
};