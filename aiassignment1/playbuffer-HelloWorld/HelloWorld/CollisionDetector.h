#pragma once
#include "Play.h"
#include "constants.h"

struct Collision
{
	Collision()
	{
		position = Play::Point2D(0, 0);
		normal = Play::Point2D(0, 0);
	};
	Play::Point2D position;
	Play::Point2D normal;
};

struct Wall
{
	Wall()
	{
		start.x = rand() % (DISPLAY_WIDTH - 80);
		start.y = rand() % (DISPLAY_HEIGHT - 80);
		end.x = start.x + (rand() % 80);
		end.y = start.y + (rand() % 80);

		//start.x = DISPLAY_WIDTH / 2;
		//end.x = start.x;
		//start.y = 0;
		//end.y = DISPLAY_HEIGHT;

		//start.x = 0;
		//end.x = DISPLAY_WIDTH;
		//start.y = DISPLAY_HEIGHT/2;
		//end.y = start.y;
	};
	void Draw();
	Play::Point2D start;
	Play::Point2D end;
};

class CollisionDetector
{
public:
	CollisionDetector();
	Wall* walls;
	Collision GetCollision(Play::Point2D position, Play::Point2D moveAmount);
	bool IsZero(float n);
	float DotProduct(Play::Point2D v1, Play::Point2D v2);
};

