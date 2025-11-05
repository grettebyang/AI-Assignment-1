#include "AI.h"
#include "Play.h"
#include "constants.h"

using namespace std;

AI::AI()
{
	pos = Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
	speed = 3;
	velocity = Play::Point2D(0, 0);
	acc = 3;
	rotation = 0;
}

void AI::Draw()
{
	Play::DrawCircle(pos, 2, Play::Colour(Play::cRed));
	Play::DrawLine(pos, pos + 4 * Play::Point2D(cosf(rotation), sinf(rotation)), Play::cRed);
}

void AI::Simulate(float elapsedTime, int activeFunction, Play::Point2D tar)
{
	target = tar;

	//Reset AI position
	if (Play::KeyPressed(Play::KEY_ENTER))
	{
		//Randomize position
		int randX = rand() % DISPLAY_WIDTH;
		int randY = rand() % DISPLAY_HEIGHT;
		float randR = (rand() % 628)/100;
		pos.x = randX;
		pos.y = randY;
		rotation = randR;
	}

	switch (activeFunction)
	{
		case 0: velocity = Seek().linear; break;
		case 1: velocity += Flee().linear; break;
		case 2: velocity += Pursue().linear; break;
		case 3: velocity += Evade().linear; break;
		case 4: velocity += Arrive().linear; break;
		case 5: velocity += Wander().linear; break;
		default: break;
	}

	if (velocity.Length() > 0)
	{
		velocity.Normalize();
		//Adjust direction facing
		rotation = atan2(velocity.y, velocity.x);
	}

	//Update position
	pos.x += velocity.x;
	pos.y += velocity.y;
}

SteeringOutput AI::Seek()
{
	SteeringOutput result;

	result.linear = target - pos;

	result.linear.Normalize();
	result.linear *= acc;

	result.angular = 0;

	return result;
}

SteeringOutput AI::Flee()
{
	SteeringOutput result;

	result.linear = pos - target;

	result.linear.Normalize();
	result.linear *= acc;

	result.angular = 0;

	return result;
}

SteeringOutput AI::Pursue()
{
	SteeringOutput result;
	return result;

}

SteeringOutput AI::Evade()
{
	SteeringOutput result;
	return result;

}

SteeringOutput AI::Arrive()
{
	SteeringOutput result;
	return result;
}

SteeringOutput AI::Wander()
{
	SteeringOutput result;

	result.linear = speed * Play::Point2D(cosf(rotation), sinf(rotation));

	result.angular = ((rand() % 157) - (rand() % 157)) / 100;
	rotation = result.angular;
	return result;
}