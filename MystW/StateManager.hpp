#pragma once
#include <memory>
#include <vector>
#include "State.hpp"
#include <SFML/Graphics.hpp>

class StateManager
{
private:
	std::vector<std::unique_ptr<State>> states;
	sf::RenderWindow* window; // Thêm con tr? window
	sf::View* uiView;         // Thêm con tr? uiView
public:
	StateManager(sf::RenderWindow* win, sf::View* view)
		: window(win), uiView(view) {
	}
	void pushState(std::unique_ptr<State> state)
	{
		states.push_back(std::move(state));
	}
	void popState()
	{
		if (!states.empty())
			states.pop_back();
	}
	State* getCurrentState()
	{
		return states.empty() ? nullptr : states.back().get();
	}

	void render()
	{
		// Luôn clear c?a s? tr??c khi v?
		window->clear();

		// S?a l?i logic render cho an toàn và d? hi?u h?n
		// Ch? state trên cùng m?i ???c v?, ho?c các state t? quy?t ??nh v? state bên d??i.
		// Logic render 2 state c?a b?n khá ??c thù cho Pause, ta có th? s?a nó trong PausePage
		if (!states.empty()) {
			states.back()->render(*window);
		}
	}

	void update(float delta)
	{
		if (!states.empty())
			states.back()->update(delta); // update top page
	}

	// Thêm các hàm getter ?? các state có th? l?y ???c window và uiView
	sf::RenderWindow* getWindow() { return window; }
	sf::View* getUiView() { return uiView; }
};