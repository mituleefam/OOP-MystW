#include "AudioManager.hpp"
#include <iostream>

AudioManager* AudioManager::instance = nullptr;

AudioManager* AudioManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new AudioManager();
	}
	return instance;
}

AudioManager::AudioManager() : globalVolume(50.0f)
{
	if (!menuMusic.openFromFile("music/Menu.wav"))
		std::cerr << "Cannot open file music menu! \n";
	menuMusic.setLoop(true);

	if (!gameMusic1.openFromFile("music/Ancient_Fortress.wav"))
		std::cerr << "Cannot open file music game 1! \n";
	gameMusic1.setLoop(true);

	if (!gameMusic2.openFromFile("music/Magical_Place.wav"))
		std::cerr << "Cannot open file music game 2! \n";
	gameMusic2.setLoop(true);

	if (!gameMusic3.openFromFile("music/Final_Chapter.wav"))
		std::cerr << "Cannot open file music game 3! \n";
	gameMusic3.setLoop(true);

	if (!winMusic.openFromFile("music/winning_sound.wav"))
		std::cerr << "Cannot open file music win! \n";
	winMusic.setLoop(true);

	if (!loseMusic.openFromFile("music/lose_sound.wav"))
		std::cerr << "Cannot open file music lose! \n";
	loseMusic.setLoop(true);
}
void AudioManager::playMenuMusic()
{
	if (gameMusic1.getStatus() == sf::Music::Playing)
		gameMusic1.stop();
	if (gameMusic2.getStatus() == sf::Music::Playing)
		gameMusic2.stop();
	if (gameMusic3.getStatus() == sf::Music::Playing)
		gameMusic3.stop();
	if (menuMusic.getStatus() != sf::Music::Playing)
		menuMusic.play();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playGameMusic1()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic1.getStatus() != sf::Music::Playing)
		gameMusic1.play();
	if (gameMusic2.getStatus() == sf::Music::Playing)
		gameMusic2.stop();
	if (gameMusic3.getStatus() == sf::Music::Playing)
		gameMusic3.stop();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playGameMusic2()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic1.getStatus() == sf::Music::Playing)
		gameMusic1.stop();
	if (gameMusic2.getStatus() != sf::Music::Playing)
		gameMusic2.play();
	if (gameMusic3.getStatus() == sf::Music::Playing)
		gameMusic3.stop();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playGameMusic3()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic1.getStatus() == sf::Music::Playing)
		gameMusic1.stop();
	if (gameMusic2.getStatus() == sf::Music::Playing)
		gameMusic2.stop();
	if (gameMusic3.getStatus() != sf::Music::Playing)
		gameMusic3.play();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playWinMusic()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic1.getStatus() == sf::Music::Playing)
		gameMusic1.stop();
	if (gameMusic2.getStatus() == sf::Music::Playing)
		gameMusic2.stop();
	if (gameMusic3.getStatus() == sf::Music::Playing)
		gameMusic3.stop();
	if (winMusic.getStatus() != sf::Music::Playing)
		winMusic.play();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playLoseMusic()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic1.getStatus() == sf::Music::Playing)
		gameMusic1.stop();
	if (gameMusic2.getStatus() == sf::Music::Playing)
		gameMusic2.stop();
	if (gameMusic3.getStatus() == sf::Music::Playing)
		gameMusic3.stop();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() != sf::Music::Playing)
		loseMusic.play();
}

void AudioManager::stopAllMusic()
{
	menuMusic.stop();
	gameMusic1.stop();
	gameMusic2.stop();
	gameMusic3.stop();
	loseMusic.stop();
	winMusic.stop();
}

void AudioManager::setGlobalVolume(const float& volume)
{
	globalVolume = volume;
	if (globalVolume < 0.0f)
		globalVolume = 0.f;
	if (globalVolume > 100.0f)
		globalVolume = 100.f;
	menuMusic.setVolume(globalVolume);
	gameMusic1.setVolume(globalVolume);
	gameMusic2.setVolume(globalVolume);
	gameMusic3.setVolume(globalVolume);
	winMusic.setVolume(globalVolume);
	loseMusic.setVolume(globalVolume);
}

float AudioManager::getGlobalVolume() const
{
	return globalVolume;
}
