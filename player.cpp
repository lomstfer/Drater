#include "Player.hpp"

/* Player::Entity(SDL_Texture* etex, int srcW, int srcH, float ex, float ey, int ew, int eh)
{
	tex = etex;

    x = ex;
    y = ey;
    w = ew;
    h = eh;
    
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = srcW;
    srcRect.h = srcH;

    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = w;
    dstRect.h = h;
} */

Player::Player(SDL_Texture *ptex, float px, float py, float pw, float ph)
{
    tex = ptex;
    w = pw;
    h = ph;
    x = px - w / 2;
    y = py - h / 2;
	
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 8;
	srcRect.h = 12;

    rect.x = ftint(x);
    rect.y = ftint(y);
    rect.w = ftint(w);
    rect.h = ftint(h);

	scale = 0;

	// the simulated position
	s_x = x;
	s_y = y;

    currentFrame = 0;
	animationSpeed = 10.0f;
	flip = false;

    speed = 30.0f;
	speedX = 0.0f;
	speedY = 0.0f;
    damp = 0.001f;


    // used to go to the last frame when up key is released
	endFramer = false;

	r = 255;
	g = 255;
	b = 255;
	alpha = 255;

	Time = 0;
}

void Player::update(int winW, int winH, double deltaTime, const Uint8 *keys)
{
	Time += deltaTime / 100.0f;
    // make the current position the last simulated position, 
    // simulated used to prevent light/small tunneling but does not prevent tunneling 
    x = s_x;
    y = s_y;

	if (currentFrame > 7)
	{
		currentFrame = 0;
	}

	if (keys[SDL_SCANCODE_UP])
	{
		speedY += -speed;
	}

	if (keys[SDL_SCANCODE_DOWN])
	{
		speedY += speed;
	}

	if (keys[SDL_SCANCODE_LEFT])
	{
		speedX += -speed;
		flip = false;
	}

	if (keys[SDL_SCANCODE_RIGHT])
	{
		speedX += speed;
		flip = true;
	}

	/*if (keys[SDL_SCANCODE_UP] && keys[SDL_SCANCODE_LEFT])
	{
		speedY += -speed * (1 / sqrt(2));
		speedX += -speed * (1 / sqrt(2));
	}

	if (keys[SDL_SCANCODE_UP] && keys[SDL_SCANCODE_RIGHT])
	{
		speedY += -speed * (1 / sqrt(2));
		speedX += speed * (1 / sqrt(2));
	}

	if (keys[SDL_SCANCODE_DOWN] && keys[SDL_SCANCODE_LEFT])
	{
		speedY += speed * (1 / sqrt(2));
		speedX += -speed * (1 / sqrt(2));
	}

	if (keys[SDL_SCANCODE_DOWN] && keys[SDL_SCANCODE_RIGHT])
	{
		speedY += speed * (1 / sqrt(2));
		speedX += speed * (1 / sqrt(2));
	}*/

	// slow down x speed
	speedX *= float(pow(damp, deltaTime));
	if (fabsf(speedX) < 0.1)
	{
		speedX = 0;
	}

	// slow down y speed
	speedY *= float(pow(damp, deltaTime));
	if (fabsf(speedY) < 0.1)
	{
		speedY = 0;
	}

	// add values to simulated position
	s_x += speedX * float(deltaTime);
	s_y += speedY * float(deltaTime);
	rect.x = ftint(x);
	rect.y = ftint(y);
}

void Player::updatePos(double deltaTime)
{
	x = s_x;
	y = s_y;
	speedX *= float(pow(damp, deltaTime));
	if (fabsf(speedX) < 0.1)
	{
		speedX = 0;
	}

	// slow down y speed
	speedY *= float(pow(damp, deltaTime));
	if (fabsf(speedY) < 0.1)
	{
		speedY = 0;
	}
	// add values to simulated position
	s_x += speedX * float(deltaTime);
	s_y += speedY * float(deltaTime);
	rect.x = ftint(x);
	rect.y = ftint(y);
	rect.w = ftint(w);
	rect.h = ftint(h);
}

void Player::render(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, tex, &srcRect, &rect);
}

void Player::noExplore(int winW, int winH)
{
	// left stop
	if (s_x + rect.w/4.0f <= 0)
	{
		s_x = 0 - rect.w/4.0f;
	}

	// right stop
	if (s_x + rect.w - rect.w/4.0f >= winW)
	{
		s_x = winW - rect.w + rect.w/4.0f;
	}

	// up stop
	if (s_y + rect.h/4.0f <= 0)
	{
		s_y = 0 - rect.h/4.0f;
	}

	// down stop
	if (s_y + rect.h - rect.h/4.0f >= winH )
	{
		s_y = winH - rect.h + rect.h/4.0f;
	}
}
