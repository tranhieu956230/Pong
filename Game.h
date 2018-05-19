#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <math.h>
#include <sstream>
using namespace sf;



class Game
{
private:
	static const int FRAMES_PER_SECOND = 60;
	static const int MAX_FRAMESKIP = 10;
	static const int width = 640;
	static const int height = 480;
	static const int borderSize = 12;
	static const int margin = 50;
	static const int moveDistance = 20;


	RenderWindow window;
	Font font;

	RectangleShape top;
	RectangleShape left;
	RectangleShape right;
	RectangleShape bottom;

	RectangleShape ball;

	Vector2f ballSpeed;

	RectangleShape player1;
	RectangleShape player2;

	RectangleShape middleLine;

	Text title;
	Text start;
	Text won;
	Text lost;
	Text score;
	Text fps;

	SoundBuffer soundBuffer1;
	Sound blip;
	SoundBuffer soundBuffer2;
	Sound blam;
	SoundBuffer soundBuffer3;
	Sound blap;
	SoundBuffer soundBuffer4;
	Sound blop;

	Time time;
	Int32 updateTime;

	unsigned int p1Score, p2Score;

	enum states { INTRO, PLAYING, P1WON, P1LOST };

	int gameState;

public:
	Game();
	~Game();
	bool init();

	int exec();

	void processEvents();

	void update();

	void setup();

	void display();

	void updatePlayer1();

	void updatePlayer2();

	void updateBall();

	void checkCollisions();


	void resetGame1();


	void resetGame2();
};

