#include "AI.h"
#include "Play.h"
#include "constants.h"
#include "Path.h"
#include "CollisionDetector.h"
#include "BlendedSteering.h"

using namespace std;

SteeringOutput Steering::GetSteering(int index, Entity character, Entity* targets, Play::Point2D targetPosition, Play::Point2D com)
{
	SteeringOutput result;
	result.linear = Play::Point2D(0, 0);
	result.angular = 0;
	return result;
}

void Flocking::Draw()
{
	for (int i = 0; i < charCount; i++)
	{
		Play::DrawCircle(characters[i].position, 2, Play::Colour(Play::cGreen));
		Play::DrawLine(characters[i].position, characters[i].position + 4 * Play::Point2D(cosf(characters[i].orientation), sinf(characters[i].orientation)), Play::cGreen);
	}
	Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), ("AI: " + to_string(charCount)).c_str(), Play::cWhite, true);
	//Play::DrawCircle(centerOfMass, 2, Play::Colour(Play::cYellow));
}

void Flocking::Update(float elapsedTime)
{
	if (Play::KeyPressed(Play::KEY_UP) && charCount < 16)
	{
		charCount++;
	}
	else if (Play::KeyPressed(Play::KEY_DOWN) && charCount > 1)
	{
		charCount--;
	}

	for (int i = 0; i < charCount; i++)
	{
		SteeringOutput steering;
		steering = GetSteering(i);
		characters[i].velocity += steering.linear * elapsedTime;
		characters[i].rotation += steering.angular * elapsedTime;

		if (characters[i].velocity.Length() > 0)
		{
			characters[i].velocity.Normalize();
			characters[i].velocity *= speed;
			//Adjust direction facing
			characters[i].orientation = atan2(characters[i].velocity.y, characters[i].velocity.x);
		}

		//Update position
		characters[i].position.x += characters[i].velocity.x * elapsedTime;
		characters[i].position.y += characters[i].velocity.y * elapsedTime;
		//if (characters[i].position.x > DISPLAY_WIDTH)
		//{
		//	characters[i].position.x -= DISPLAY_WIDTH;
		//	//pos[i].y = DISPLAY_HEIGHT - pos[i].y;
		//}
		//else if (characters[i].position.x < 0)
		//{
		//	characters[i].position.x += DISPLAY_WIDTH;
		//	//pos[i].y = DISPLAY_HEIGHT - pos[i].y;
		//}
		//if (characters[i].position.y > DISPLAY_HEIGHT)
		//{
		//	characters[i].position.y -= DISPLAY_HEIGHT;
		//	//pos[i].x = DISPLAY_WIDTH - pos[i].x;
		//}
		//else if (characters[i].position.y < 0)
		//{
		//	characters[i].position.y += DISPLAY_HEIGHT;
		//	//pos[i].x = DISPLAY_WIDTH - pos[i].x;
		//}

		characters[i].orientation += characters[i].rotation * elapsedTime;
	}
}

SteeringOutput Flocking::GetSteering(int index)
{
	SteeringOutput result;
	SetCenterOfMass();
	for (int i = 0; i < behaviorsCount; i++)
	{
		SteeringOutput curSteering = behaviors[i].behavior->GetSteering(index, characters[index], characters, target->pos, centerOfMass);
		result.linear += curSteering.linear * behaviors[i].weight;
		result.angular += curSteering.angular * behaviors[i].weight;
	}
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), to_string(result.linear.Length()).c_str(), Play::cWhite, true);
	if (result.linear.Length() > maxAcc)
	{
		result.linear.Normalize();
		result.linear *= maxAcc;
	}
	result.angular = min(result.angular, maxRot);

	return result;
}

SteeringOutput Separation::GetSteering(int index, Entity character, Entity* targets, Play::Point2D targetPosition, Play::Point2D com)
{
	SteeringOutput result;
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), "get steering", Play::cWhite, true);

	for (int i = 0; i < charCount; i++)
	{
		if (index != i)
		{
			Play::Point2D direction = targets[i].position - character.position;
			float distance = direction.Length();
			if (distance < threshold)
			{
				//strength[index] = min(.7f * distance * distance, acc[index]); //Inverse Square Law
				strength[index] = character.acceleration * (threshold - distance) / threshold; //Linear Separation
				direction.Normalize();
				result.linear = -1 * strength[index] * direction;
			}
		}
	}
	result.angular = 0;

	return result;
}

SteeringOutput AlignAndMatchVelocity::GetSteering(int index, Entity character, Entity* targets, Play::Point2D targetPosition, Play::Point2D com)
{
	SteeringOutput result;
	targetVelocity = targetPosition - com;
	targetVelocity.Normalize();
	targetVelocity *= 40;

	float rot = atan2(targetVelocity.y, targetVelocity.x) - character.orientation;
	rot = MapToRange(rot);
	float rotSize = abs(rot);

	if (rotSize < targetRadius)
	{
		result.linear = Play::Point2D(0, 0);
		result.angular = 0;
		character.velocity = Play::Point2D(0, 0);
		character.rotation = 0;
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
	result.angular -= character.rotation;
	result.angular /= timeToTarget;

	float angularAcc = abs(result.angular);
	if (angularAcc > maxAngularAcc)
	{
		result.angular /= angularAcc;
		result.angular *= maxAngularAcc;
	}

	result.linear = targetVelocity - character.velocity;
	result.linear /= timeToTarget;

	return result;
}

float AlignAndMatchVelocity::MapToRange(float rot)
{
	float newRot = rot;

	if (abs(rot) > 2 * acos(0.0))
	{
		newRot = (-1 * (rot / abs(rot))) * ((acos(0.0) * 4) - abs(rot));
	}

	return newRot;
}

SteeringOutput Cohesion::GetSteering(int index, Entity character, Entity* targets, Play::Point2D targetPosition, Play::Point2D com)
{
	SteeringOutput result;
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 12), (to_string(centerOfMass.x) + ", " + to_string(centerOfMass.y)).c_str(), Play::cWhite, true);

	result.linear = com - character.position;
	result.angular = 0;

	return result;
}

void Flocking::SetCenterOfMass()
{
	centerOfMass = Play::Point2D(0, 0);
	for (int i = 0; i < charCount; i++)
	{
		centerOfMass += characters[i].position;
	}
	centerOfMass /= float(charCount);
}