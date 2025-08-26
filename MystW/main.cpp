#include <SFML/Graphics.hpp>
#include "StateManager.hpp"
#include "GameState.hpp"
#include "MenuState.hpp"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Mysterious World", sf::Style::Default);
    //sf::View fixedView(sf::FloatRect(0, 0, 1920, 1080));
    //window.setView(fixedView);
    // Đổi tên fixedView thành uiView cho rõ nghĩa.
    // Đây là view DÀNH RIÊNG cho tất cả các giao diện người dùng.
    sf::View uiView(sf::FloatRect(0, 0, 1920, 1080));
    window.setFramerateLimit(60); // LIMIT FPS

    StateManager manager(&window, &uiView); // Truyền uiView vào StateManager
    // manager.pushState(std::make_unique<MenuState>(&manager, &window));
    manager.pushState(std::make_unique<MenuState>(&manager));
    AudioManager::getInstance()->setGlobalVolume(50.0f);
    AudioManager::getInstance()->playMenuMusic();

    sf::Clock clock;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized) {
                float windowRatio = static_cast<float>(event.size.width) / event.size.height;
                float viewRatio = 1920.0f / 1080.0f;

                //sf::View view(sf::FloatRect(0, 0, 1920, 1080));

                // KHÔNG TẠO VIEW MỚI.
                // CHỈ CẬP NHẬT VIEWPORT TRÊN uiView HIỆN CÓ.
                if (windowRatio > viewRatio) {
                    float width = viewRatio / windowRatio;
                    uiView.setViewport(sf::FloatRect((1.0f - width) / 2.0f, 0.0f, width, 1.0f));
                }
                else {
                    float height = windowRatio / viewRatio;
                    uiView.setViewport(sf::FloatRect(0.0f, (1.0f - height) / 2.0f, 1.0f, height));
                }
                // Dòng window.setView ở đây không còn cần thiết nữa vì ta sẽ set view trong từng state.
                //window.setView(view);
            }

            if (manager.getCurrentState())
                manager.getCurrentState()->handleEvent(event);
        }

        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        if (manager.getCurrentState())
            manager.update(deltaTime);

        window.clear();
        if (manager.getCurrentState())
            // manager.render(window);
            manager.render();

        window.display();
    }

    return 0;
}
