#include "Player.h"
#include "Play.h"
#include "constants.h"

using namespace std;

Player::Player()
{
	pos = Play::Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
	maxSpeed = 100;
	velocity = Play::Point2D(0, 0);
	rotation = 0;
}

void Player::Draw()
{
	Play::DrawCircle(pos, 2, Play::Colour(Play::cBlue));
	//Play::DrawDebugText(Play::Point2D(DISPLAY_WIDTH / 2, 30), to_string(rotation).c_str(), Play::cWhite, true);
	Play::DrawLine(pos, pos + 4*Play::Point2D(cosf(rotation), sinf(rotation)), Play::cBlue);
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
		rotation = atan2(velocity.y, velocity.x);
	}
	velocity *= maxSpeed;

	//Player Movement
	pos.y += velocity.y * elapsedTime;
	pos.x += velocity.x * elapsedTime;
}