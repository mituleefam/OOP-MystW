#pragma once
#include "State.hpp"
#include "StateManager.hpp"
#include "Spirit.h"
#include "Player.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class RewardState : public State {
private:
    StateManager* states;
    sf::RenderWindow* win;
    sf::Font font;
    sf::Text message;
    sf::RectangleShape panel;
    sf::Clock timer;

    Spirit* spiritRef; // Con trỏ đến spirit của player

public:
    RewardState(StateManager* sm, Spirit* spirit)
        : states(sm), win(sm->getWindow()), spiritRef(spirit)
    {
        font.loadFromFile("font/bitcount.ttf");

        message.setFont(font);
        message.setString("You have unlocked the Spirit!");
        message.setCharacterSize(50);
        message.setFillColor(sf::Color::Yellow);
        message.setPosition(400, 300);

        panel.setSize({ 800, 400 });
        panel.setFillColor(sf::Color(0, 0, 0, 180));
        panel.setOrigin(panel.getSize().x / 2, panel.getSize().y / 2);
        panel.setPosition(1920 / 2, 1080 / 2);
    }

    void handleEvent(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
            giveRewardAndReturn();
        }
    }

    void update(float delta) override {
        if (timer.getElapsedTime().asSeconds() > 3.f) {
            giveRewardAndReturn();
        }
    }

    void render(sf::RenderWindow& window) override {
        window.draw(panel);
        window.draw(message);
    }

private:
    void giveRewardAndReturn() {
        if (spiritRef) {
            spiritRef->activate();
        }
        states->popState(); // Thoát khỏi RewardState → quay về GameState
    }
};
