#include <SDL.h>
#include <math.h>
#include "Ftint.hpp"
#include "Player.hpp"

class WaterPixel : public Player
{
public:
	using Player::Player;

	int decreaseVar = 0;

	void decrease(double deltaTime)
	{
		decreaseVar += 1 * deltaTime;
		if (decreaseVar >= 1){
			decreaseVar = 0;
			w = w - 10;
			h = h - 10;
		}
		
	}
};