#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "constants.h"
#include "GameManager.h"
#include "Player.h"
#include "AI.h"
#include "Path.h"
#include "BlendedSteering.h"

using namespace std;

GameManager gm;
Player p;
AI ai;
Flocking f;

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	srand(time(0));
	ai = AI();
	f = Flocking(10, &p);
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	Play::ClearDrawingBuffer(Play::cBlack);

	gm.Frame(elapsedTime);
	gm.DrawData();

	p.Simulate(elapsedTime);
	p.Draw();

	if (gm.mode) 
	{
		ai.Simulate(elapsedTime, gm.activeFunction, p);
		ai.Draw();
		if (gm.activeFunction == 6)
		{
			ai.path.Draw();
		}
		else if (gm.activeFunction == 9)
		{
			if (Play::KeyPressed(KEY_SPACE))
			{
				ai.colDet.RandomizeWalls();
			}
			for (int i = 0; i < 3; i++)
			{
				ai.colDet.walls[i].Draw();
				//p.colDet.walls[i].Draw();
			}
		}
	}
	else
	{
		f.Update(elapsedTime);
		f.Draw();
	}

	Play::PresentDrawingBuffer();
	return Play::KeyDown( KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

