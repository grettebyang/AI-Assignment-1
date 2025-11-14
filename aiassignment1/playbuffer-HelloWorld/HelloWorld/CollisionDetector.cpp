#include "CollisionDetector.h"
#include "Play.h"
#include "constants.h"

using namespace std;

CollisionDetector::CollisionDetector()
{
	walls = new Wall[3];
}

void Wall::Draw()
{
	Play::DrawLine(start, end, Play::cMagenta);
}

Collision CollisionDetector::GetCollision(Play::Point2D position, Play::Point2D moveAmount)
{
	Collision col;
	Play::Point2D curCol;
	Play::Point2D endPoint = position + moveAmount;
	float startX;
	float startY;
	float endX;
	float endY;
	if (endPoint.x <= position.x)
	{
		endX = position.x;
		startX = endPoint.x;
		endY = position.y;
		startY = endPoint.y;
	}
	else
	{
		endX = endPoint.x;
		startX = position.x;
		endY = endPoint.y;
		startY = position.y;
	}
	//Play::DrawLine(position, endPoint, Play::cYellow);
	//Play::DrawCircle(Play::Point2D(endX, endY), 4, Play::cGrey);
	//Play::DrawCircle(Play::Point2D(startX, startY), 4, Play::cCyan);

	for (int i = 0; i < 3; i++)
	{
		//Play::DrawCircle(walls[i].start, 4, Play::cCyan);
		//Play::DrawCircle(walls[i].end, 4, Play::cCyan);
		float aWall = (walls[i].end.y - walls[i].start.y);
		float bWall = -(walls[i].end.x - walls[i].start.x);
		float cWall = (bWall * walls[i].start.y) - (aWall * walls[i].start.x);
		float wallInt = walls[i].start.y + ((aWall / bWall) * walls[i].start.x);
		float mWall = -(aWall / bWall);
		float aRay = (endY - startY);
		float bRay = -(endX - startX);
		float cRay = (bRay * startY) - (aRay * startX);
		float rayInt = startY + ((aRay / bRay) * startX);
		float mRay = -(aRay / bRay);

		if (bRay != 0 && bWall != 0)
		{
			col.position.x = (rayInt - wallInt) / (mWall - mRay);
			col.position.y = mWall * col.position.x + wallInt;
		}
		else if (bRay == 0)
		{
			col.position.x = startX;
			col.position.y = mWall * col.position.x + wallInt;
		}
		else if (bWall == 0)
		{
			col.position.x = walls[i].start.x;
			col.position.y = mRay * col.position.x + rayInt;
		}

		//Play::DrawDebugText(Play::Point2D(100, 16 * (i + 1)), to_string(aRay).c_str(), Play::cWhite, true);

		if (!IsZero(bWall) && ((aWall >= 0 && (col.position.x > walls[i].end.x || col.position.x < walls[i].start.x))))
		{
			//Play::DrawCircle(col.position, 4, Play::cYellow);
			continue;
		}
		else if (!IsZero(bRay) && ((aRay >= 0 && (col.position.x > endX || col.position.x < startX))))
		{
			//Play::DrawCircle(col.position, 4, Play::cBlue);
			continue;
		}
		else if (!IsZero(aWall) && ((mWall > 0 && (col.position.y > walls[i].end.y || col.position.y < walls[i].start.y))))
		{
			//Play::DrawCircle(col.position, 4, Play::cYellow);
			continue;
		}
		else if (!IsZero(aRay) && ((mRay > 0 && (col.position.y > endY || col.position.y < startY)) || (mRay < 0 && (col.position.y < endY || col.position.y > startY))))
		{
			//Play::DrawCircle(col.position, 4, Play::cRed);
			continue;
		}

		if (col.position.x == col.position.x && col.position.y == col.position.y && (col.position - position).Length() < (curCol - position).Length())
		{
			//Play::DrawCircle(col.position, 4, Play::cYellow);
			float nx = 0;
			float ny = 0;
			if (!IsZero(aWall))
			{
				nx = col.position.x + 15;
				ny = (bWall / aWall) * 15 + col.position.y;
				Play::Point2D d = position - col.position;
				if (DotProduct(d, Play::Point2D(nx, ny) - col.position) < 0)
				{
					nx = col.position.x - 15;
					ny = (bWall / aWall) * -15 + col.position.y;
				}
			}
			else
			{
				nx = col.position.x;
				ny = col.position.y + 15;
				Play::Point2D d = position - col.position;
				if (DotProduct(d, Play::Point2D(nx, ny) - col.position) < 0)
				{
					ny = -15 + col.position.y;
				}
			}
			col.normal = Play::Point2D(nx, ny) - col.position;
			//Play::DrawLine(col.position, Play::Point2D(nx, ny), Play::cOrange);
			col.normal.Normalize();
			curCol = col.position;
		}
	}
	col.position = curCol;

	return col;
}

bool CollisionDetector::IsZero(float n)
{
	if (n > -.001 && n < .001)
	{
		return true;
	}
	return false;
}

float CollisionDetector::DotProduct(Play::Point2D v1, Play::Point2D v2)
{
	float dot;

	dot = v1.x * v2.x + v1.y * v2.y;
	//Play::DrawDebugText(Play::Point2D(100, 16), to_string(dot).c_str(), Play::cWhite, true);

	return dot;
}

void CollisionDetector::RandomizeWalls()
{
	for (int i = 0; i < 3; i++)
	{
		walls[i].start.x = rand() % (DISPLAY_WIDTH - 40);
		walls[i].start.y = rand() % DISPLAY_HEIGHT;
		walls[i].end.x = walls[i].start.x + (rand() % 80);
		walls[i].end.y = walls[i].start.y + ((rand() % 80) - (rand() % 160));
	}
}