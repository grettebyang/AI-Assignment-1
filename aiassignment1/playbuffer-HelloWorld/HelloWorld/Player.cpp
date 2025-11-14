#include "Player.h"
#include "Play.h"
#include "constants.h"
#include "CollisionDetector.h"

using namespace std;

Player::Player()
{
	pos = Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
	maxSpeed = 100;
	velocity = Play::Point2D(0, 0);
	orientation = 0;
	colDet = CollisionDetector();
}

void Player::Draw()
{
	Play::DrawCircle(pos, 2, Play::Colour(Play::cBlue));
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 30), to_string(orientation).c_str(), Play::cWhite, true);
	Play::DrawLine(pos, pos + 4*Play::Point2D(cosf(orientation), sinf(orientation)), Play::cBlue);
}

void Player::Simulate(float elapsedTime)
{
	//Player input
	velocity.y = Play::KeyDown(Play::KEY_W) - Play::KeyDown(Play::KEY_S);
	velocity.x = Play::KeyDown(Play::KEY_D) - Play::KeyDown(Play::KEY_A);

	if (velocity.Length() > 0)
	{
		velocity.Normalize();
		//Adjust direction facing
		orientation = atan2(velocity.y, velocity.x);
	}
	velocity *= maxSpeed;

	//Player Movement
	pos.y += velocity.y * elapsedTime;
	pos.x += velocity.x * elapsedTime;

	//Wall collision test
	Play::Point2D rayVec = Play::Point2D(cosf(orientation), sinf(orientation));
	rayVec.Normalize();
	rayVec *= 25;
	Collision col = colDet.GetCollision(pos, rayVec);
	if (col.normal.Length() > 0)
	{
		//Play::DrawCircle(col.position, 4, Play::cYellow);
	}
}