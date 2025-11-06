#include "AI.h"
#include "Play.h"
#include "constants.h"

using namespace std;

AI::AI()
{
	pos = Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
	speed = 100;
	velocity = Play::Point2D(0, 0);
	acc = 500;
	rotation = 0;
	targetRotation = 0;
	orientation = 0;
	tarPos = Play::Point2D(0, 0);
	wanderOrientation = 0;
}

void AI::Draw()
{
	Play::DrawCircle(pos, 2, Play::Colour(Play::cRed));
	Play::DrawLine(pos, pos + 4 * Play::Point2D(cosf(orientation), sinf(orientation)), Play::cRed);
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), (to_string(velocity.x) + ", " + to_string(velocity.y)).c_str(), Play::cWhite, true);
}

void AI::Simulate(float elapsedTime, int activeFunction, Player tar)
{
	target = tar;
	tarPos = target.pos;

	//Reset AI position
	if (Play::KeyPressed(Play::KEY_ENTER))
	{
		//Randomize position
		int randX = rand() % DISPLAY_WIDTH;
		int randY = rand() % DISPLAY_HEIGHT;
		float randR = (rand() % 628)/100;
		pos.x = randX;
		pos.y = randY;
		orientation = randR;
	}

	SteeringOutput steering;
	switch (activeFunction)
	{
		case 0: steering = Seek(); break;
		case 1: steering = Flee(); break;
		case 2: steering = Pursue(); break;
		case 3: steering = Evade(); break;
		case 4: steering = Arrive(); break;
		case 5: steering = Wander(); break;
		default: break;
	}
	velocity += steering.linear * elapsedTime;
	rotation += steering.angular * elapsedTime;

	if (velocity.Length() > 0)
	{
		velocity.Normalize();
		velocity *= speed;
		//Adjust direction facing
		orientation = atan2(velocity.y, velocity.x);
	}

	//Update position
	pos.x += velocity.x * elapsedTime;
	pos.y += velocity.y * elapsedTime;

	orientation += rotation * elapsedTime;
}

SteeringOutput AI::Seek()
{
	SteeringOutput result;

	result.linear = tarPos - pos;

	if (result.linear.Length() > 0)
	{
		result.linear.Normalize();
	}
	result.linear *= acc;

	result.angular = 0;

	return result;
}

SteeringOutput AI::Flee()
{
	SteeringOutput result;

	result.linear = pos - tarPos;

	if (result.linear.Length() > 0)
	{
		result.linear.Normalize();
	}
	result.linear *= acc;

	result.angular = 0;

	return result;
}

SteeringOutput AI::Pursue()
{
	SteeringOutput result;
	float maxPred = 100;
	float pred = 0;

	Play::Point2D direction = tarPos - pos;
	float distance = direction.Length();
	float curSpd = velocity.Length();

	if (curSpd <= distance / maxPred)
	{
		pred = maxPred;
	}
	else
	{
		pred = distance / curSpd;
	}

	tarPos += target.velocity * pred;

	return Seek();
}

SteeringOutput AI::Evade()
{
	SteeringOutput result;
	float maxPred = 100;
	float pred = 0;

	Play::Point2D direction = tarPos - pos;
	float distance = direction.Length();
	float curSpd = velocity.Length();

	if (curSpd <= distance / maxPred)
	{
		pred = maxPred;
	}
	else
	{
		pred = distance / curSpd;
	}

	tarPos += target.velocity * pred;
	
	return Flee();
}

SteeringOutput AI::Arrive()
{
	SteeringOutput result;
	float targetRadius = 6;
	float slowRadius = 30;
	float timeToTarget = .25;
	float targetSpeed = 0;

	Play::Point2D direction = tarPos - pos;
	float distance = direction.Length();

	if (distance < targetRadius)
	{
		velocity = Play::Point2D(0, 0);
		rotation = 0;
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		return result;
	}

	if (distance < slowRadius)
	{
		targetSpeed = speed;
	}
	else
	{
		targetSpeed = speed * distance / slowRadius;
	}

	result.linear = direction;
	result.linear.Normalize();
	result.linear *= targetSpeed;

	result.linear -= velocity;
	result.linear /= timeToTarget;

	if (result.linear.Length() > acc)
	{
		result.linear.Normalize();
		result.linear *= acc;
	}

	result.angular = 0;
	return result;
}

SteeringOutput AI::Wander()
{
	SteeringOutput result;
	float wanderOffset = 60;
	float wanderRadius = 15;
	float wanderRate = .9;
	float maxAcc = 300;

	wanderOrientation += ((float(rand() % 100) - float(rand() % 100)) / 100) * wanderRate;
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), to_string(wanderOrientation).c_str(), Play::cWhite, true);
	float targetOrientation = wanderOrientation + orientation;

	Play::Point2D oriVec = Play::Point2D(cosf(orientation), sinf(orientation));
	Play::Point2D tarOriVec = Play::Point2D(cosf(targetOrientation), sinf(targetOrientation));
	tarPos = pos + wanderOffset * oriVec;
	Play::DrawLine(pos, tarPos, Play::cWhite);
	Play::DrawCircle(tarPos, wanderRadius, Play::cWhite);
	tarPos += wanderRadius * tarOriVec;
	Play::DrawCircle(tarPos, 3, Play::cGreen);


	result = Face();

	result.linear = maxAcc * oriVec;

	return result;
}

SteeringOutput AI::Face()
{
	SteeringOutput result;

	Play::Point2D direction = tarPos - pos;

	if (direction.Length() == 0)
	{
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		velocity = Play::Point2D(0, 0);
		rotation = 0;
		return result;
	}

	direction.Normalize();
	targetRotation = atan2(direction.y, direction.x);

	return Align();
}

SteeringOutput AI::Align()
{
	SteeringOutput result;

	float maxAngularAcc = 500;
	float maxRot = 20;
	float targetRadius = .1;
	float slowRadius = .2;
	float timeToTarget = .25;

	float rot = targetRotation - orientation;
	rot = MapToRange(rot);
	float rotSize = abs(rot);

	if (rotSize < targetRadius)
	{
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		velocity = Play::Point2D(0, 0);
		rotation = 0;
		return result;
	}

	if (rotSize > slowRadius)
	{
		result.angular = maxRot;
	}
	else
	{
		result.angular = maxRot * rotSize / slowRadius;
	}

	result.angular *= rot / rotSize;
	result.angular -= rotation;
	result.angular /= timeToTarget;

	float angularAcc = abs(result.angular);
	if (angularAcc > maxAngularAcc)
	{
		result.angular /= angularAcc;
		result.angular *= maxAngularAcc;
	}

	result.linear = tarPos - pos;

	if (result.linear.Length() > 0)
	{
		result.linear.Normalize();
	}
	result.linear *= acc;

	return result;
}

float AI::MapToRange(float rot)
{
	float newRot = rot;

	if (abs(rot) > 2 * acos(0.0))
	{
		newRot = (-1 * (rot/abs(rot))) * ((acos(0.0) * 4) - abs(rot));
	}

	return newRot;
}