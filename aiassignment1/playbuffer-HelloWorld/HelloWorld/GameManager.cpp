#include "GameManager.h"
#include "constants.h"
#include "Play.h"

using namespace std;

GameManager::GameManager()
{
	activeFunction = 0;
}

void GameManager::Frame(float elapsedTime)
{
	//Get Function input
	if (Play::KeyPressed(Play::KEY_1))
	{
		activeFunction = 0;
	}
	else if (Play::KeyPressed(Play::KEY_2))
	{
		activeFunction = 1;
	}
	else if (Play::KeyPressed(Play::KEY_3))
	{
		activeFunction = 2;
	}
	else if (Play::KeyPressed(Play::KEY_4))
	{
		activeFunction = 3;
	}
	else if (Play::KeyPressed(Play::KEY_5))
	{
		activeFunction = 4;
	}
	else if (Play::KeyPressed(Play::KEY_6))
	{
		activeFunction = 5;
	}
}

void GameManager::DrawData()
{
	//Draw Functions UI
	string n = to_string(activeFunction + 1);
	const char *func;
	const char *actFunc = n.c_str();

	switch (activeFunction)
	{
		case 0: func = "Seek"; break;
		case 1: func = "Flee"; break;
		case 2: func = "Pursue"; break;
		case 3: func = "Evade"; break;
		case 4: func = "Arrive"; break;
		case 5: func = "Wander"; break;
		default: func = ""; break;
	}

	Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 15), actFunc, Play::cWhite, true);
	Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 30), func, Play::cWhite, true);
}
