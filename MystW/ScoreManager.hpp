#pragma once
#include <vector>
#include <string>

class ScoreManager
{
private:
	int score = 0;
	static std::vector<int> scores;
	static const std::string fileName;
public:
	ScoreManager();
	static void loadScore();
	static void addScore(int score);
	static std::vector<int> getTopScore();
	static void saveScore();

	// ADDED
	void incrementScore(int amount);
	void resetScore();
	int getScore() const;
};