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

    sf::Texture spiritTexture;
    sf::Sprite spiritSprite;
    sf::IntRect currentFrame;
    sf::Clock animationTimer;
    int frameWidth;
    int frameHeight;
    int numFrames;
    float animationSpeed;

public:
    RewardState(StateManager* sm, Spirit* spirit)
        : states(sm), win(sm->getWindow()), spiritRef(spirit)
    {
        font.loadFromFile("font/bitcount.ttf");

        message.setFont(font);
        message.setString("You have unlocked the Spirit!");
        message.setCharacterSize(50);
        message.setFillColor(sf::Color::Yellow);
        message.setPosition(500, 300);

        panel.setSize({ 800, 500 });
        panel.setFillColor(sf::Color(0, 0, 0, 180));
        panel.setOrigin(panel.getSize().x / 2, panel.getSize().y / 2);
        panel.setPosition(1920 / 2, 1080 / 2);

        if (!spiritTexture.loadFromFile("image/spirit/Meow-Knight_Idle.png")) {
            message.setString("Error: Spirit image not found!");
        }

        spiritSprite.setTexture(spiritTexture);

 
        numFrames = 6;
        frameWidth = spiritTexture.getSize().x;
        frameHeight = spiritTexture.getSize().y / numFrames; 
        animationSpeed = 0.15f; 


        currentFrame = sf::IntRect(0, 0, frameWidth, frameHeight);
        spiritSprite.setTextureRect(currentFrame);

      
        spiritSprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
        spiritSprite.setScale(8.f, 8.f); 
        spiritSprite.setPosition(panel.getPosition().x, panel.getPosition().y + 50);
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
        if (animationTimer.getElapsedTime().asSeconds() > animationSpeed) {
            currentFrame.top += frameHeight;

            if (currentFrame.top >= frameHeight * numFrames) {
                currentFrame.top = 0;
            }

            spiritSprite.setTextureRect(currentFrame);

            animationTimer.restart();
        }
    }

    void render(sf::RenderWindow& window) override {
        window.draw(panel);
        window.draw(message);
        window.draw(spiritSprite);
    }

private:
    void giveRewardAndReturn() {
        if (spiritRef) {
            spiritRef->activate();
        }
        states->popState(); // Thoát khỏi RewardState → quay về GameState
    }
};
