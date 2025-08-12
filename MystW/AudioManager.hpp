#pragma once
#include <SFML/Audio.hpp>

class AudioManager
{
public:
	static AudioManager* getInstance();

	void playMenuMusic();
	void playGameMusic();
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
	sf::Music gameMusic;
	sf::Music menuMusic;
	sf::Music winMusic;
	sf::Music loseMusic;

	float globalVolume;
};