#pragma once
#include "Play.h"
#include "Player.h"
#include "Path.h"
#include "CollisionDetector.h"

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

	Play::Point2D* pos;
	Player target;
	Path path;
	Play::Point2D tarPos;
	Play::Point2D* velocity;
	float *acc;
	float speed;
	float* rotation;
	float* orientation;
	float* targetRotation;
	float* wanderOrientation;
	int aiCount;
	float colRadius;
	
	float curParam;
	float* strength;
	float threshold;

	CollisionDetector colDet;
	float avoidDist;
	float lookAhead;

	//Steering functions
	SteeringOutput Seek(int index);
	SteeringOutput Flee(int index);
	SteeringOutput Pursue(int index);
	SteeringOutput Evade(int index);
	SteeringOutput Arrive(int index);
	SteeringOutput Wander(int index);

	SteeringOutput Face(int index);
	SteeringOutput Align(int index);
	float MapToRange(float rot);

	//Path Following
	SteeringOutput FollowPath(int index);

	//Separation
	SteeringOutput Separation(int index);

	//Collision Avoidance
	SteeringOutput CollisionAvoidance(int index);

	//Obstacle Avoidance
	SteeringOutput ObstacleAvoidance(int index);
};

