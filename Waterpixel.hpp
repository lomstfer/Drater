#include <SDL.h>
#include <math.h>
#include "Ftint.hpp"
#include "Player.hpp"
#include "iostream"
class WaterPixel : public Player
{
public:
	using Player::Player;

	float decreaseVar = 0;

	void decrease(double deltaTime)
	{
		
		decreaseVar += 1.0f * deltaTime;
		if (decreaseVar >= 1){
			decreaseVar = 0;
			w -= 1.0f;
			h -= 1.0f;
			s_x += 0.5f;
			s_y += 0.5f;
		}
		
	}
};