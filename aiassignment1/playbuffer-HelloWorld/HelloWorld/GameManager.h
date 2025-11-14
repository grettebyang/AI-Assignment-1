#pragma once
#include "Play.h"

class GameManager
{
public:
	GameManager();

	void Frame(float elapsedTime);
	void DrawData();
	int activeFunction;
	bool mode;
	const char* func;
};
