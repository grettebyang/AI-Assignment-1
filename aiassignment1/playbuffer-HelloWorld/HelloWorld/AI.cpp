#include "AI.h"
#include "Play.h"
#include "constants.h"
#include "Path.h"
#include "CollisionDetector.h"

using namespace std;

AI::AI()
{
	speed = 80;
	tarPos = Play::Point2D(0, 0);
	aiCount = 1;
	path = Path(8);
	curParam = 0;
	threshold = 25;
	colRadius = 4;
	colDet = CollisionDetector();
	avoidDist = 15;
	lookAhead = 50;

	int n = 16;
	pos = new Play::Point2D[n];
	velocity = new Play::Point2D[n];
	acc = new float[n];
	rotation = new float[n];
	targetRotation = new float[n];
	orientation = new float[n];
	wanderOrientation = new float[n];
	strength = new float[n];
	for (int i = 0; i < n; i++)
	{
		//Randomize position
		int randX = rand() % DISPLAY_WIDTH;
		int randY = rand() % DISPLAY_HEIGHT;
		float randR = (rand() % 628) / 100;
		orientation[i] = randR;
		pos[i] = Play::Point2D(randX, randY);
		velocity[i] = Play::Point2D(0, 0);
		acc[i] = 3000;
		rotation[i] = 0;
		targetRotation[i] = 0;
		wanderOrientation[i] = 0;
		strength[i] = 0;
	}
}

void AI::Draw()
{
	for (int i = 0; i < aiCount; i++)
	{
		Play::DrawCircle(pos[i], 2, Play::Colour(Play::cRed));
		Play::DrawLine(pos[i], pos[i] + 4 * Play::Point2D(cosf(orientation[i]), sinf(orientation[i])), Play::cRed);
	}
	Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), ("AI:" + to_string(aiCount)).c_str(), Play::cWhite, true);
}

void AI::Simulate(float elapsedTime, int activeFunction, Player tar)
{
	target = tar;
	if (Play::KeyPressed(Play::KEY_UP) && aiCount < 16)
	{
		aiCount++;
	}
	else if (Play::KeyPressed(Play::KEY_DOWN) && aiCount > 1)
	{
		aiCount--;
	}

	for(int i = 0; i < aiCount; i++)
	{
		//Reset AI position
		if (Play::KeyPressed(Play::KEY_ENTER))
		{
			//Randomize position
			int randX = rand() % DISPLAY_WIDTH;
			int randY = rand() % DISPLAY_HEIGHT;
			float randR = (rand() % 628) / 100;
			pos[i].x = randX;
			pos[i].y = randY;
			orientation[i] = randR;
		}

		SteeringOutput steering;
		switch (activeFunction)
		{
		case 0: tarPos = target.pos; steering = Seek(i); break;
		case 1: tarPos = target.pos; steering = Flee(i); break;
		case 2: tarPos = target.pos; steering = Pursue(i); break;
		case 3: tarPos = target.pos; steering = Evade(i); break;
		case 4: tarPos = target.pos; steering = Arrive(i); break;
		case 5: steering = Wander(i); break;
		case 6: steering = FollowPath(i); break;
		case 7: steering = Separation(i); break;
		case 8: steering = CollisionAvoidance(i); break;
		case 9: steering = ObstacleAvoidance(i); break;
		default: break;
		}
		velocity[i] += steering.linear * elapsedTime;
		rotation[i] += steering.angular * elapsedTime;

		if (velocity[i].Length() > 0)
		{
			velocity[i].Normalize();
			velocity[i] *= speed;
			//Adjust direction facing
			orientation[i] = atan2(velocity[i].y, velocity[i].x);
		}

		//Update position
		pos[i].x += velocity[i].x * elapsedTime;
		pos[i].y += velocity[i].y * elapsedTime;
		if (pos[i].x > DISPLAY_WIDTH)
		{
			pos[i].x -= DISPLAY_WIDTH;
			//pos[i].y = DISPLAY_HEIGHT - pos[i].y;
		}
		else if (pos[i].x < 0)
		{
			pos[i].x += DISPLAY_WIDTH;
			//pos[i].y = DISPLAY_HEIGHT - pos[i].y;
		}
		if (pos[i].y > DISPLAY_HEIGHT)
		{
			pos[i].y -= DISPLAY_HEIGHT;
			//pos[i].x = DISPLAY_WIDTH - pos[i].x;
		}
		else if (pos[i].y < 0)
		{
			pos[i].y += DISPLAY_HEIGHT;
			//pos[i].x = DISPLAY_WIDTH - pos[i].x;
		}

		orientation[i] += rotation[i] * elapsedTime;
	}
}

SteeringOutput AI::Seek(int index)
{
	SteeringOutput result;

	result.linear = tarPos - pos[index];

	if (result.linear.Length() > 0)
	{
		result.linear.Normalize();
	}
	result.linear *= acc[index];

	result.angular = 0;

	return result;
}

SteeringOutput AI::Flee(int index)
{
	SteeringOutput result;

	result.linear = pos[index] - tarPos;

	if (result.linear.Length() > 0)
	{
		result.linear.Normalize();
	}
	result.linear *= acc[index];

	result.angular = 0;

	return result;
}

SteeringOutput AI::Pursue(int index)
{
	SteeringOutput result;
	float maxPred = 100;
	float pred = 0;

	Play::Point2D direction = tarPos - pos[index];
	float distance = direction.Length();
	float curSpd = velocity[index].Length();

	if (curSpd <= distance / maxPred)
	{
		pred = maxPred;
	}
	else
	{
		pred = distance / curSpd;
	}

	tarPos += target.velocity * pred;

	return Seek(index);
}

SteeringOutput AI::Evade(int index)
{
	SteeringOutput result;
	float maxPred = 100;
	float pred = 0;

	Play::Point2D direction = tarPos - pos[index];
	float distance = direction.Length();
	float curSpd = velocity[index].Length();

	if (curSpd <= distance / maxPred)
	{
		pred = maxPred;
	}
	else
	{
		pred = distance / curSpd;
	}

	tarPos += target.velocity * pred;
	
	return Flee(index);
}

SteeringOutput AI::Arrive(int index)
{
	SteeringOutput result;
	float targetRadius = 6;
	float slowRadius = 30;
	float timeToTarget = .25;
	float targetSpeed = 0;

	Play::Point2D direction = tarPos - pos[index];
	float distance = direction.Length();

	if (distance < targetRadius)
	{
		velocity[index] = Play::Point2D(0, 0);
		rotation[index] = 0;
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

	result.linear -= velocity[index];
	result.linear /= timeToTarget;

	if (result.linear.Length() > acc[index])
	{
		result.linear.Normalize();
		result.linear *= acc[index];
	}

	result.angular = 0;
	return result;
}

SteeringOutput AI::Wander(int index)
{
	SteeringOutput result;
	float wanderOffset = 60;
	float wanderRadius = 15;
	float wanderRate = .9;
	float maxAcc = 300;

	wanderOrientation[index] += ((float(rand() % 100) - float(rand() % 100)) / 100) * wanderRate;
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), to_string(wanderOrientation).c_str(), Play::cWhite, true);
	float targetOrientation = wanderOrientation[index] + orientation[index];

	Play::Point2D oriVec = Play::Point2D(cosf(orientation[index]), sinf(orientation[index]));
	Play::Point2D tarOriVec = Play::Point2D(cosf(targetOrientation), sinf(targetOrientation));
	tarPos = pos[index] + wanderOffset * oriVec;
	//Play::DrawLine(pos[index], tarPos, Play::cWhite);
	//Play::DrawCircle(tarPos, wanderRadius, Play::cWhite);
	tarPos += wanderRadius * tarOriVec;
	//Play::DrawCircle(tarPos, 3, Play::cGreen);


	result = Face(index);

	result.linear = maxAcc * oriVec;

	return result;
}

SteeringOutput AI::Face(int index)
{
	SteeringOutput result;

	Play::Point2D direction = tarPos - pos[index];

	if (direction.Length() == 0)
	{
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		velocity[index] = Play::Point2D(0, 0);
		rotation[index] = 0;
		return result;
	}

	direction.Normalize();
	targetRotation[index] = atan2(direction.y, direction.x);

	return Align(index);
}

SteeringOutput AI::Align(int index)
{
	SteeringOutput result;

	float maxAngularAcc = 500;
	float maxRot = 20;
	float targetRadius = .1;
	float slowRadius = .2;
	float timeToTarget = .25;

	float rot = targetRotation[index] - orientation[index];
	rot = MapToRange(rot);
	float rotSize = abs(rot);

	if (rotSize < targetRadius)
	{
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		velocity[index] = Play::Point2D(0, 0);
		rotation[index] = 0;
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
	result.angular -= rotation[index];
	result.angular /= timeToTarget;

	float angularAcc = abs(result.angular);
	if (angularAcc > maxAngularAcc)
	{
		result.angular /= angularAcc;
		result.angular *= maxAngularAcc;
	}

	result.linear = tarPos - pos[index];

	if (result.linear.Length() > 0)
	{
		result.linear.Normalize();
	}
	result.linear *= acc[index];

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

SteeringOutput AI::FollowPath(int index)
{
	SteeringOutput result;
	float pathOffset = 5;
	float predictTime = .5;

	Play::Point2D futurePos = pos[index] + (velocity[index] * predictTime);
	//Play::DrawCircle(futurePos, 4, Play::cYellow);
	curParam = path.GetParam(futurePos, curParam);

	float targetParam = curParam + pathOffset;

	tarPos = path.GetPosition(targetParam);
	//Play::DrawCircle(tarPos, 4, Play::cYellow);
	//Play::DrawCircle(path.GetPosition(curParam), 4, Play::cYellow);

	return Seek(index);
}

SteeringOutput AI::Separation(int index)
{
	SteeringOutput result;

	for (int i = 0; i < aiCount; i++)
	{
		if (index != i)
		{
			Play::Point2D direction = pos[i] - pos[index];
			float distance = direction.Length();
			if (distance < threshold)
			{
				//strength[index] = min(.7f * distance * distance, acc[index]); //Inverse Square Law
				strength[index] = acc[index] * (threshold - distance) / threshold; //Linear Separation
				direction.Normalize();
				result.linear = -1 * strength[index] * direction;
			}
		}
	}
	result.angular = 0;

	return result;
}

SteeringOutput AI::CollisionAvoidance(int index)
{
	SteeringOutput result;
	float shortestTime = .6;
	int firstTarget = -1;
	float firstMinSeparation = 0;
	float firstDistance = 0;
	Play::Point2D firstRelativePos = Play::Point2D(0, 0);
	Play::Point2D firstRelativeVel = Play::Point2D(0, 0);
	Play::Point2D relativePos = Play::Point2D(0, 0);
	float distance = 0;

	for (int i = 0; i < aiCount; i++)
	{
		if (index != i)
		{
			relativePos = pos[i] - pos[index];
			Play::Point2D relativeVel = velocity[i] - velocity[index];
			float relativeSpeed = relativeVel.Length();
			float timeToCollision = (relativePos.Dot(relativeVel)) / (relativeSpeed * relativeSpeed);

			distance = relativePos.Length();
			float minSeparation = distance - relativeSpeed * shortestTime;
			if (minSeparation > 2 * colRadius)
			{
				continue;
			}

			if (timeToCollision > 0 && timeToCollision < shortestTime)
			{
				shortestTime = timeToCollision;
				firstTarget = i;
				firstMinSeparation = minSeparation;
				firstDistance = distance;
				firstRelativePos = relativePos;
				firstRelativeVel = relativeVel;
			}
		}
	}

	if (firstTarget == -1)
	{
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		return result;
	}

	if (firstMinSeparation <= 0 || distance < (2 * colRadius))
	{
		relativePos = pos[firstTarget] - pos[index];
	}
	else
	{
		relativePos = firstRelativePos + firstRelativeVel * shortestTime;
	}

	relativePos.Normalize();
	result.linear = -1 * relativePos * acc[index];
	result.angular = 0;

	return result;
}

SteeringOutput AI::ObstacleAvoidance(int index)
{
	SteeringOutput result;
	Play::Point2D rayVec = velocity[index];
	rayVec.Normalize();
	rayVec *= lookAhead;

	Collision col = colDet.GetCollision(pos[index], rayVec);

	if (col.normal.Length() == 0)
	{
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		return Wander(index);
	}

	tarPos = col.position + col.normal * avoidDist;
	//Play::DrawCircle(tarPos, 4, Play::cYellow);

	return Seek(index);
}

