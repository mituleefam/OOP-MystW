#pragma once
#include <memory>
#include <vector>
#include "State.hpp"
#include <SFML/Graphics.hpp>

class StateManager
{
private:
	std::vector<std::unique_ptr<State>> states;
public:
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

	void render(sf::RenderWindow& window)
	{
		if (states.size() >= 2)
			states[states.size() - 2]->render(window); // render GameState then render pause page

		if (!states.empty())
			states.back()->render(window); // render PausePage or top page
	}

	void update(float delta)
	{
		if (!states.empty())
			states.back()->update(delta); // update top page
	}
};
