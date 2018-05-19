#include "Game.h"

#define PI   3.14159265358979323846

Game::Game()
{
}


Game::~Game()
{
}
bool intersects(const RectangleShape & rect1, const RectangleShape & rect2)
{
	FloatRect r1 = rect1.getGlobalBounds();
	FloatRect r2 = rect2.getGlobalBounds();
	return r1.intersects(r2);
}

float clamp(const float x, const float a, const float b)
{
	return std::min(std::max(a, x), b);
}
bool Game::init()
{
	VideoMode videoMode(width, height);
	window.create(videoMode, "Pong SFML");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(FRAMES_PER_SECOND);

	if (!font.loadFromFile("arial.ttf"))
		return false;

	if (!soundBuffer1.loadFromFile("blip.wav"))
		return false;

	if (!soundBuffer2.loadFromFile("blam.wav"))
		return false;

	if (!soundBuffer3.loadFromFile("blap.wav"))
		return false;

	if (!soundBuffer4.loadFromFile("blop.wav"))
		return false;

	setup();
	return true;
}
int Game::exec()
{
	Clock renderClock, updateClock;
	while (window.isOpen())
	{
		time = renderClock.getElapsedTime();
		float fFps = 1000000 / time.asMicroseconds();
		std::stringstream s;
		s << fFps << " fps";
		fps.setString(s.str());
		renderClock.restart();

		const Int64 frameTime = 1000000 / FRAMES_PER_SECOND;
		Clock c;
		Time t = c.getElapsedTime();
		Int64 nextFrameTime = t.asMicroseconds() + frameTime;

		int loops = 0;
		while (t.asMicroseconds() < nextFrameTime && loops < MAX_FRAMESKIP)
		{
			processEvents();
			updateTime = updateClock.restart().asMilliseconds();
			update();
			t = c.getElapsedTime();
			loops++;
		}

		display();
	}

	return EXIT_SUCCESS;
}

void Game::processEvents()
{
	Event event;
	while (window.pollEvent(event))
	{
		if ((event.type == Event::Closed) ||
			((event.type == Event::KeyPressed) && (event.key.code == Keyboard::Escape)))
			window.close();
		else
			if ((event.type == Event::KeyPressed) && (gameState == INTRO))
				gameState = PLAYING;
	}
}

void Game::update()
{
	if (gameState != PLAYING)
		return;

	updatePlayer1();
	updatePlayer2();
	checkCollisions();
	updateBall();

	// detect if game is over
	if (p1Score >= 11 && p1Score >= p2Score + 2)
		gameState = P1WON;
	if (p2Score >= 11 && p2Score >= p1Score + 2)
		gameState = P1LOST;
}

void Game::setup()
{
	top.setPosition(-2 * borderSize, 0);
	top.setSize(Vector2f(width + 4 * borderSize, borderSize));

	left.setPosition(-borderSize * 3, 0);
	left.setSize(Vector2f(borderSize, height));

	right.setPosition(width + 2 * borderSize, 0);
	right.setSize(Vector2f(borderSize, height));

	bottom.setPosition(-2 * borderSize, height - borderSize);
	bottom.setSize(Vector2f(width + 4 * borderSize, borderSize));

	top.setFillColor(Color(100, 100, 100));
	top.setOutlineColor(Color::Blue);
	top.setOutlineThickness(3);

	left.setFillColor(Color(100, 100, 100));
	left.setOutlineColor(Color::Blue);
	left.setOutlineThickness(3);

	right.setFillColor(Color(100, 100, 100));
	right.setOutlineColor(Color::Blue);
	right.setOutlineThickness(3);

	bottom.setFillColor(Color(100, 100, 100));
	bottom.setOutlineColor(Color::Blue);
	bottom.setOutlineThickness(3);

	ball.setPosition(width / 2, height / 2);
	ball.setSize(Vector2f(20, 20));
	ball.setFillColor(Color::Yellow);
	ball.setOutlineColor(Color::Red);
	ball.setOutlineThickness(2);

	player1.setSize(Vector2f(borderSize, 90));
	player1.setPosition(margin - borderSize, height / 2 - 25);
	player1.setFillColor(Color(0, 122, 245));
	player1.setOutlineColor(Color::Red);
	player1.setOutlineThickness(3);

	player2.setSize(Vector2f(borderSize, 90));
	player2.setPosition(width - margin, height / 2 - 25);
	player2.setFillColor(Color(0, 122, 245));
	player2.setOutlineColor(Color::Red);
	player2.setOutlineThickness(3);

	middleLine.setFillColor(Color(100, 100, 100, 30));
	middleLine.setOutlineColor(Color(0, 0, 100, 30));
	middleLine.setOutlineThickness(2);
	middleLine.setPosition(width / 2, 0);
	middleLine.setSize(Vector2f(0, height));

	title.setString("Pong SFML 2");
	title.setFont(font);
	title.setCharacterSize(50);
	title.setPosition(width / 2 - title.getGlobalBounds().width / 2, 100);
	title.setFillColor(Color::Blue);

	start.setString("Press any key to start");
	start.setFont(font);
	start.setCharacterSize(30);
	start.setPosition(width / 2 - start.getGlobalBounds().width / 2, 400);
	start.setFillColor(Color::Red);

	won.setString("You have won this game.\n\n Congratulations !");
	won.setFont(font);
	won.setCharacterSize(20);
	won.setPosition(width / 2 - won.getGlobalBounds().width / 2, height / 2 - won.getGlobalBounds().height / 2);
	won.setFillColor(Color::Green);

	lost.setString("You have lost this game, \n better luck next time!");
	lost.setFont(font);
	lost.setCharacterSize(20);
	lost.setPosition(width / 2 - lost.getGlobalBounds().width / 2, height / 2 - lost.getGlobalBounds().height / 2);
	lost.setFillColor(Color::Red);

	score.setString("0   0");
	score.setFont(font);
	score.setCharacterSize(50);
	score.setPosition(width / 2 - score.getGlobalBounds().width / 2, 40);
	score.setFillColor(Color(0, 0, 100, 50));

	fps.setString("0");
	fps.setFont(font);
	fps.setCharacterSize(30);
	fps.setPosition(fps.getGlobalBounds().width / 2, 40);
	fps.setFillColor(Color(52, 0, 100, 50));


	blip = Sound(soundBuffer1);
	blam = Sound(soundBuffer2);
	blap = Sound(soundBuffer3);
	blop = Sound(soundBuffer4);

	resetGame1();
	p1Score = 0;
	p2Score = 0;
	gameState = INTRO;

}

void Game::display()
{
	window.clear(Color::White);
	switch (gameState)
	{
	case INTRO:
		window.draw(title);
		window.draw(start);
		break;
	case PLAYING:
		window.draw(middleLine);
		window.draw(left);
		window.draw(right);
		window.draw(player1);
		window.draw(player2);
		window.draw(ball);
		window.draw(score);
		window.draw(top);
		window.draw(bottom);
		window.draw(fps);
		break;
	case P1WON:
		window.draw(won);
		break;
	case P1LOST:
		window.draw(lost);
		break;
	}
	window.display();
}

void Game::updatePlayer1()
{
	// move player 1 pad
	if (Keyboard::isKeyPressed(Keyboard::Up))
	{
		player1.move(0, -moveDistance * updateTime / 30.0);
	}
	else
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			player1.move(0, moveDistance*updateTime / 30.0);
		}
}

void Game::updatePlayer2()
{
	// auto move player2 pad
	if (ball.getPosition().y < player2.getPosition().y)
		player2.move(0, -moveDistance * updateTime / 30.0);
	else if (ball.getPosition().y + ball.getSize().y > player2.getPosition().y + player2.getSize().y)
		player2.move(0, moveDistance*updateTime / 30.0);
}

void Game::updateBall()
{
	ball.move(ballSpeed.x*updateTime, ballSpeed.y*updateTime);
}

void Game::checkCollisions()
{

	// block players pad inside the play area
	if (intersects(player1, bottom) || intersects(player1, top))
	{
		FloatRect t = top.getGlobalBounds();
		FloatRect b = bottom.getGlobalBounds();
		Vector2f p = player1.getPosition();
		p.y = clamp(p.y, t.top + t.height + 5, b.top - player1.getSize().y - 5);
		player1.setPosition(p);
		blap.play();
	}
	if (intersects(player2, bottom) || intersects(player2, top))
	{
		FloatRect t = top.getGlobalBounds();
		FloatRect b = bottom.getGlobalBounds();
		Vector2f p = player2.getPosition();
		p.y = clamp(p.y, t.top + t.height + 5, b.top - player2.getSize().y - 5);
		player2.setPosition(p);
		blap.play();
	}
	// ball collides with top and bottom
	if (intersects(ball, top))
	{
		FloatRect t = top.getGlobalBounds();
		FloatRect b = ball.getGlobalBounds();
		ballSpeed.y *= -1;
		int u = t.top + t.height - b.top;
		ball.move(0, 2 * u);
		blop.play();
	}
	if (intersects(ball, bottom))
	{
		FloatRect bot = bottom.getGlobalBounds();
		FloatRect b = ball.getGlobalBounds();
		ballSpeed.y *= -1;
		int u = bot.top - b.height - b.top;
		ball.move(0, 2 * u);
		blop.play();
	}
	// ball collides with player1 and player2
	if (intersects(ball, player1))
	{
		FloatRect p = player1.getGlobalBounds();
		FloatRect b = ball.getGlobalBounds();

		//let o be the center of p
		Vector2f o = Vector2f(p.left + p.width / 2, p.top + p.height / 2);
		//om: vector from o to the center of the ball
		Vector2f om = Vector2f(b.left + b.width / 2 - o.x, b.top + b.height / 2 - o.y);
		// let's scale om to square dimensions and act as if p is a square
		om.x /= p.width;
		om.y /= p.height;
		// reflect the ball according to the angle of om
		float angle = atan2(om.y, om.x);
		if (abs(angle) < PI / 2) //right - PI/4 is restrictive
		{
			ballSpeed.x = abs(ballSpeed.x);//in case of double contact
			ballSpeed.y = (b.top + b.height / 2 - p.top - p.height / 2) / 100;
			int u = p.left + p.width - b.left;
			b.left = p.left + p.width + u;
			ball.setPosition(b.left, b.top);
			//increase ball speed by 2%
			ballSpeed.x *= 1.02f;
			ballSpeed.y *= 1.02f;
			blip.play();
		}
	}
	if (intersects(ball, player2))
	{
		FloatRect p = player2.getGlobalBounds();
		FloatRect b = ball.getGlobalBounds();

		//let o be the center of p
		Vector2f o = Vector2f(p.left + p.width / 2, p.top + p.height / 2);
		//om: vector from o to the center of the ball
		Vector2f om = Vector2f(b.left + b.width / 2 - o.x, b.top + b.height / 2 - o.y);
		// let's scale om to square dimensions and act as if p is a square
		om.x /= p.width;
		om.y /= p.height;
		// reflect the ball according to the angle of om
		float angle = atan2(om.y, om.x);
		if (abs(angle) > PI / 2) //left - 3*PI/4 is restrictive
		{
			ballSpeed.x = -abs(ballSpeed.x);//in case of double contact
			ballSpeed.y = (b.top + b.height / 2 - p.top - p.height / 2) / 100;
			int u = b.left + b.width - p.left;
			b.left = p.left - b.width - u;
			ball.setPosition(b.left, b.top);
			//increase ball speed by 2%
			ballSpeed.x *= 1.02f;
			ballSpeed.y *= 1.02f;
			blip.play();
		}
	}

	// check for scoring
	if (intersects(ball, left))
	{
		blam.play();
		p2Score++;
		std::stringstream str;
		str << p1Score << "   " << p2Score;
		score.setString(str.str());
		score.setPosition(width / 2 - score.getGlobalBounds().width / 2, 40);
		resetGame2();
	}
	if (intersects(ball, right))
	{
		blam.play();
		p1Score++;
		std::stringstream str;
		str << p1Score << "   " << p2Score;
		score.setString(str.str());
		score.setPosition(width / 2 - score.getGlobalBounds().width / 2, 40);
		resetGame1();
	}

}
void Game::resetGame1()
{
	FloatRect p = player1.getGlobalBounds();
	FloatRect b = ball.getGlobalBounds();
	ball.setPosition(p.left + p.width + 5, height / 2);
	ballSpeed.x = 0.6f;
	ballSpeed.y = 0.6f;
}

void Game::resetGame2()
{
	FloatRect p = player2.getGlobalBounds();
	FloatRect b = ball.getGlobalBounds();
	ball.setPosition(p.left - b.width - 5, height / 2);
	ballSpeed.x = -0.6f;
	ballSpeed.y = 0.6f;
}
