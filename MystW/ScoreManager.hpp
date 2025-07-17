#pragma once
#include <vector>
#include <string>

class ScoreManager
{
private:
	static std::vector<int> scores;
	static const std::string fileName;
	ScoreManager();
public:
	static void loadScore();
	static void addScore(int score);
	static std::vector<int> getTopScore();
	static void saveScore();
};