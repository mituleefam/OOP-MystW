#include "ScoreManager.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>

const std::string ScoreManager::fileName = "scores.txt";
std::vector<int> ScoreManager::scores;
void ScoreManager::loadScore()
{
	scores.clear();
	std::ifstream input(fileName);
	if (!input.is_open())
	{
		std::cout << "Cannot open file";
		return;
	}
	int score;
	while (input >> score)
		scores.push_back(score);
	std::sort(scores.begin(), scores.end(), std::greater<>());
	if (scores.size() >= 3)
		scores.resize(3);
	input.close();
}

void ScoreManager::addScore(int score)
{
	loadScore();
	scores.push_back(score);
	std::sort(scores.begin(), scores.end(), std::greater<>());
	if (scores.size() >= 3)
		scores.resize(3);
	saveScore();
}

void ScoreManager::saveScore()
{
	std::ofstream out(fileName);
	if (!out.is_open())
	{
		std::cout << "Cannot open file";
		return;
	}
	for (int score : scores)
		out << score << std::endl;
	out.close();
}

std::vector<int> ScoreManager::getTopScore()
{
	loadScore();
	return scores;
}