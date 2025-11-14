#include "GameManager.h"
#include "constants.h"
#include "Play.h"

using namespace std;

GameManager::GameManager()
{
	activeFunction = 0;
	mode = true;
	func = "Seek";
}

void GameManager::Frame(float elapsedTime)
{
	//Get Function input
	if (mode)
	{
		if (Play::KeyPressed(Play::KEY_1))
		{
			activeFunction = 0;
			func = "Seek";
		}
		else if (Play::KeyPressed(Play::KEY_2))
		{
			activeFunction = 1;
			func = "Flee";
		}
		else if (Play::KeyPressed(Play::KEY_3))
		{
			activeFunction = 2;
			func = "Pursue";
		}
		else if (Play::KeyPressed(Play::KEY_4))
		{
			activeFunction = 3;
			func = "Evade";
		}
		else if (Play::KeyPressed(Play::KEY_5))
		{
			activeFunction = 4;
			func = "Arrive";
		}
		else if (Play::KeyPressed(Play::KEY_6))
		{
			activeFunction = 5;
			func = "Wander";
		}
		else if (Play::KeyPressed(Play::KEY_7))
		{
			activeFunction = 6;
			func = "Follow Path";
		}
		else if (Play::KeyPressed(Play::KEY_8))
		{
			activeFunction = 7;
			func = "Separation";
		}
		else if (Play::KeyPressed(Play::KEY_9))
		{
			activeFunction = 8;
			func = "Collision Avoidance";
		}
		else if (Play::KeyPressed(Play::KEY_0))
		{
			activeFunction = 9;
			func = "Obstacle Avoidance";
		}
		else if (Play::KeyPressed(Play::KEY_SHIFT))
		{
			mode = false;
			func = "Flocking";
		}
	}
	else if (Play::KeyPressed(Play::KEY_SHIFT))
	{
		mode = true;
		switch (activeFunction)
		{
		case 0: func = "Seek"; break;
		case 1: func = "Flee"; break;
		case 2: func = "Pursue"; break;
		case 3: func = "Evade"; break;
		case 4: func = "Arrive"; break;
		case 5: func = "Wander"; break;
		case 6: func = "Follow Path"; break;
		case 7: func = "Separation"; break;
		case 8: func = "Collision Avoidance"; break;
		case 9: func = "Obstacle Avoidance"; break;
		default: func = ""; break;
		}
	}
}

void GameManager::DrawData()
{
	//Draw Functions UI
	if (mode)
	{
		Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 15), to_string(activeFunction + 1).c_str(), Play::cWhite, true);
		Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 30), func, Play::cWhite, true);
	}
	else
	{
		Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 15), func, Play::cWhite, true);
	}
}
