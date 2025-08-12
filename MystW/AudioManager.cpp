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
	if (!gameMusic.openFromFile("music/Background_2.wav"))
		std::cerr << "Cannot open file music game! \n";
	gameMusic.setLoop(true);
	if (!winMusic.openFromFile("music/winning_sound.wav"))
		std::cerr << "Cannot open file music win! \n";
	winMusic.setLoop(true);
	if (!loseMusic.openFromFile("music/lose_sound.wav"))
		std::cerr << "Cannot open file music lose! \n";
	loseMusic.setLoop(true);
}
void AudioManager::playMenuMusic()
{
	if (gameMusic.getStatus() == sf::Music::Playing)
		gameMusic.stop();
	if (menuMusic.getStatus() != sf::Music::Playing)
		menuMusic.play();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playGameMusic()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic.getStatus() != sf::Music::Playing)
		gameMusic.play();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playWinMusic()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic.getStatus() == sf::Music::Playing)
		gameMusic.stop();
	if (winMusic.getStatus() != sf::Music::Playing)
		winMusic.play();
	if (loseMusic.getStatus() == sf::Music::Playing)
		loseMusic.stop();
}

void AudioManager::playLoseMusic()
{
	if (menuMusic.getStatus() == sf::Music::Playing)
		menuMusic.stop();
	if (gameMusic.getStatus() == sf::Music::Playing)
		gameMusic.stop();
	if (winMusic.getStatus() == sf::Music::Playing)
		winMusic.stop();
	if (loseMusic.getStatus() != sf::Music::Playing)
		loseMusic.play();
}

void AudioManager::stopAllMusic()
{
	menuMusic.stop();
	gameMusic.stop();
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
	gameMusic.setVolume(globalVolume);
	winMusic.setVolume(globalVolume);
	loseMusic.setVolume(globalVolume);
}

float AudioManager::getGlobalVolume() const
{
	return globalVolume;
}
