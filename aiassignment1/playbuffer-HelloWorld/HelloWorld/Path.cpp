#include "Play.h"
#include "Path.h"
#include "constants.h"

Path::Path()
{
	length = 0;
	int n = rand() % 10;
	nodes = new Play::Point2D[n];
	for (int i = 0; i < n; i++)
	{
		//Randomize position
		int randX = rand() % DISPLAY_WIDTH;
		int randY = rand() % DISPLAY_HEIGHT;
		nodes[i].x = randX;
		nodes[i].y = randY;
		if (i > 0)
		{
			length += (nodes[i] - nodes[i - 1]).Length();
		}
		length += (nodes[0] - nodes[n - 1]).Length();
	}
	nodeCount = n;

}
Path::Path(int n)
{
	length = 0;
	float rad = 80;
	nodes = new Play::Point2D[n];
	for (int i = 0; i < n; i++)
	{
		//Randomize position
		float randX = (DISPLAY_WIDTH/2) + cosf((i + 1) * acos(0.0)*4/n) * rad;
		float randY = (DISPLAY_HEIGHT/2) + sinf((i + 1) * acos(0.0) * 4 / n) * rad;
		nodes[i].x = randX;
		nodes[i].y = randY;
		if (i > 0)
		{
			length += (nodes[i] - nodes[i - 1]).Length();
		}
	}
	length += (nodes[0] - nodes[n - 1]).Length();
	nodeCount = n;
}

void Path::Draw()
{
	for (int i = 0; i < nodeCount-1; i++)
	{
		Play::DrawLine(nodes[i], nodes[i + 1], Play::cOrange);
	}
	Play::DrawLine(nodes[0], nodes[nodeCount - 1], Play::cOrange);
}

float Path::GetParam(Play::Point2D position, float lastParam)
{
	//returns nearest point on path to position
	float param = 0;
	float curParamLen = 0;
	float minDistance = (nodes[0] - position).Length();

	for (int i = 0; i < nodeCount; i++)
	{
		int secondNodeIndex = (i + 1) % nodeCount;
		Play::Point2D curLineLen = nodes[secondNodeIndex] - nodes[i];
		Play::Point2D direction = curLineLen;
		Play::Point2D nodeToPos = position - nodes[i];
		direction.Normalize();
		Play::Point2D dotLen = nodeToPos.Dot(direction) * direction;
		Play::Point2D midPos = nodes[i] + dotLen;
		float midDist = (position - midPos).Length();
		if (minDistance > midDist)
		{
			minDistance = midDist;
			param = curParamLen + dotLen.Length();
		}
		curParamLen += curLineLen.Length();
	}

	if (param == lastParam)
	{
		param += 1;
		if (param > length)
		{
			param = param - length;
		}
	}
	return param;
}

Play::Point2D Path::GetPosition(float param)
{
	Play::Point2D pos = nodes[0];
	float curParamLen = 0;
	if (param > length)
	{
		param -= length;
	}
	else if (param == length || param == 0)
	{
		return nodes[0];
	}

	for (int i = 0; i < nodeCount; i++)
	{
		int secondNodeIndex = (i + 1) % nodeCount;
		float curLineLen = (nodes[i] - nodes[secondNodeIndex]).Length();
		if (curParamLen + curLineLen > param)
		{
			Play::Point2D direction = nodes[secondNodeIndex] - nodes[i];
			direction.Normalize();
			pos = nodes[i] + (direction * (param - curParamLen));
			return pos;
		}
		curParamLen += curLineLen;
	}

	return pos;
}