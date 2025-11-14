#pragma once
#include "Play.h"
#include "Player.h"
#include "AI.h"
#include "constants.h"

using namespace std;

struct Entity
{
public:
	Entity()
	{
		//Randomize position
		int randX = rand() % DISPLAY_WIDTH;
		int randY = rand() % DISPLAY_HEIGHT;
		float randR = (rand() % 628) / 100;
		velocity = Play::Point2D(rand() % 10, rand() % 10);
		orientation = randR;
		acceleration = 500;
		rotation = 0;
		position = Play::Point2D(randX, randY);
	};
	Play::Point2D velocity;
	float orientation;
	float acceleration;
	float rotation;
	Play::Point2D position;
};

class Steering
{
public:
	Steering(int count) : charCount(count) {};
	int charCount;
	virtual SteeringOutput GetSteering(int index, Entity character, Entity* targets, Play::Point2D targetPosition, Play::Point2D com);
};

struct BehaviorAndWeight
{
public:
	BehaviorAndWeight(Steering *b, float w) : behavior(b), weight(w) {};
	Steering *behavior;
	float weight;
};

class Separation : public Steering
{
public:
	Separation(int count) : Steering(count), charCount(count), threshold(25), strength(new float[count]{0}) {};
	int charCount;
	float* strength;
	float threshold;
	SteeringOutput GetSteering(int index, Entity character, Entity *targets, Play::Point2D targetPosition, Play::Point2D com);
};
class AlignAndMatchVelocity : public Steering
{
public:
	AlignAndMatchVelocity(int count) : Steering(count), charCount(count) 
	{
		maxAngularAcc = 500;
		maxRot = 200;
		targetRadius = .1;
		slowRadius = .2;
		timeToTarget = .25f;
		targetVelocity = Play::Point2D(10, 10);
	};
	int charCount;
	float maxAngularAcc;
	float maxRot;
	float targetRadius;
	float slowRadius;
	float timeToTarget;
	Play::Point2D targetVelocity;

	SteeringOutput GetSteering(int index, Entity character, Entity* targets, Play::Point2D targetPosition, Play::Point2D com);
	float MapToRange(float rot);
};
class Cohesion : public Steering
{
public:
	Cohesion(int count) : Steering(count), charCount(count) {};
	int charCount;
	SteeringOutput GetSteering(int index, Entity character, Entity* targets, Play::Point2D targetPosition, Play::Point2D com);
};

class Flocking
{
public:
	Flocking()
	{
		charCount = 0;
		behaviorsCount = 0;
		speed = 80;
		maxAcc = 600;
		maxRot = 200;
	};
	Flocking(int count, Player *target)
	{
		this->target = target;
		charCount = min(count, 16);
		behaviorsCount = 3;
		behaviors = new BehaviorAndWeight[3](BehaviorAndWeight(new Separation(charCount), 1.5f), BehaviorAndWeight(new AlignAndMatchVelocity(charCount), 1.5f), BehaviorAndWeight(new Cohesion(charCount), 1.5f));
		characters = new Entity[16];
		for (int i = 0; i < 16; i++)
		{
			characters[i] = Entity();
		}
		speed = 40;
		maxAcc = 600;
		maxRot = 200;
		centerOfMass = Play::Point2D(0,0);
	};
	Player *target;
	BehaviorAndWeight* behaviors;
	int behaviorsCount;
	Entity* characters;
	int charCount;
	float speed;
	float maxAcc;
	float maxRot;
	Play::Point2D centerOfMass;
	SteeringOutput GetSteering(int index);
	void Update(float elapsedTime);
	void Draw();
	void SetCenterOfMass();
};
