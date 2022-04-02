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

	void decrease(double amount)
	{
		alpha -= amount;
		SDL_SetTextureAlphaMod(tex, alpha);
	}
};