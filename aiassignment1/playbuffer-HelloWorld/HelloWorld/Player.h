#pragma once
#include "Play.h"

class Player
{
public:
	Player();

	void Draw();
	void Simulate(float elapsedTime);

	Play::Point2D pos;
	Play::Point2D velocity;
	float maxSpeed;
	float rotation;

};