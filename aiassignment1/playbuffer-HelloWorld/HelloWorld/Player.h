#pragma once
#include "Play.h"
#include "CollisionDetector.h"

class Player
{
public:
	Player();

	void Draw();
	void Simulate(float elapsedTime);

	Play::Point2D pos;
	Play::Point2D velocity;
	float maxSpeed;
	float orientation;
	CollisionDetector colDet;
};