#include <SFML/Graphics.hpp>
#include "StateManager.hpp"
#include "GameState.hpp"
#include "MenuState.hpp"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "My Game", sf::Style::Default);
    sf::View fixedView(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(fixedView);
    window.setFramerateLimit(60); // LIMIT FPS

    StateManager manager;
    manager.pushState(std::make_unique<MenuState>(&manager, &window));

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

                sf::View view(sf::FloatRect(0, 0, 1920, 1080));

                if (windowRatio > viewRatio) {
                    float width = viewRatio / windowRatio;
                    view.setViewport(sf::FloatRect((1.0f - width) / 2.0f, 0.0f, width, 1.0f));
                }
                else {
                    float height = windowRatio / viewRatio;
                    view.setViewport(sf::FloatRect(0.0f, (1.0f - height) / 2.0f, 1.0f, height));
                }

                window.setView(view);
            }

            if (manager.getCurrentState())
                manager.getCurrentState()->handleEvent(event);
        }

        float deltaTime = clock.restart().asSeconds();

        if (manager.getCurrentState())
            manager.update(deltaTime);

        window.clear();
        if (manager.getCurrentState())
            manager.render(window);

        window.display();
    }

    return 0;
}
