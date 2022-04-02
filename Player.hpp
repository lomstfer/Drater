#ifndef _PLAYER_
#define _PLAYER_

#include <SDL.h>
#include <math.h>
#include "Ftint.hpp"


class Player
{
public:
	//using Entity::Entity;
    SDL_Texture* tex;
    float x;
    float y;
    float w;
    float h;
	float s_x;
	float s_y;

	SDL_Rect srcRect;
    SDL_Rect rect;

	float scale;

    float currentFrame;
	float animationSpeed;
	bool flip;

    float speed;
	float speedX;
	float speedY;
    float damp;

    // used to go to the last frame when up key is released
	bool endFramer;

	int r;
	int g;
	int b;
	float alpha;

	double Time;

	Player(SDL_Texture *ptex, int px, int py, float pw, float ph);

	Player() = default;

    void update(int winW, int winH, double deltaTime, const Uint8 *keys);

	void updatePos(double deltaTime);

	void render(SDL_Renderer* renderer);

	void noExplore(int winW, int winH);
};

#endif _PLAYER_