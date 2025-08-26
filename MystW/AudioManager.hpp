#pragma once
#include <SFML/Audio.hpp>

class AudioManager
{
public:
	static AudioManager* getInstance();

	void playMenuMusic();
	void playGameMusic1();
	void playGameMusic2();
	void playGameMusic3();
	void playWinMusic();
	void playLoseMusic();
	void stopAllMusic();
	void setGlobalVolume(const float& volume);
	float getGlobalVolume() const;
private:
	AudioManager();

	AudioManager(const AudioManager&) = delete;
	void operator = (const AudioManager&) = delete;
	static AudioManager* instance;
	sf::Music gameMusic1;
	sf::Music gameMusic2;
	sf::Music gameMusic3;
	sf::Music menuMusic;
	sf::Music winMusic;
	sf::Music loseMusic;

	float globalVolume;
};