#pragma once
#include "Play.h"

class Kinematic
{
public:
	Kinematic();
	void Update();
	void Draw();

	Play::Point2D position;
	Play::Point2D velocity;
	float orientation;
	float rotation;
};