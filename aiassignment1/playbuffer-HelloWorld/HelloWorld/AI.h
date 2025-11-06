#pragma once
#include "Play.h"
#include "Player.h"

struct KinematicSteeringOutput
{
	KinematicSteeringOutput()
	{
		velocity = Play::Point2D(0,0);
		rotation = 0;
	};
	Play::Point2D velocity;
	float rotation;
};

struct SteeringOutput
{
	SteeringOutput()
	{
		linear = Play::Point2D(0, 0);
		angular = 0;
	};
	Play::Point2D linear;
	float angular;
};

class AI
{
public:
	AI();
	
	void Draw();
	void Simulate(float elapsedTime, int activeFunction, Player tar);

	Play::Point2D pos;
	Player target;
	Play::Point2D tarPos;
	Play::Point2D velocity;
	float acc;
	float speed;
	float rotation;
	float orientation;
	float targetRotation;
	float wanderOrientation;

	//Steering functions
	SteeringOutput Seek();
	SteeringOutput Flee();
	SteeringOutput Pursue();
	SteeringOutput Evade();
	SteeringOutput Arrive();
	SteeringOutput Wander();

	SteeringOutput Face();
	SteeringOutput Align();
	float MapToRange(float rot);
};
