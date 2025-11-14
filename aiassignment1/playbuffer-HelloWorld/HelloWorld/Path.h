#pragma once
#include "Play.h"

class Path 
{
public:
	Path(int n);
	Path();
	float GetParam(Play::Point2D position, float lastParam);
	Play::Point2D GetPosition(float param);
	void Draw();

	float length;
	int nodeCount;
	Play::Point2D* nodes;
};