#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <fstream>

#include "Player.hpp"
#include "Entity.hpp"
#include "Text.hpp"
#include "Ftint.hpp"
#include "Waterpixel.hpp"


#define Log(x) std::cout << x << std::endl;

void ifquit(bool &game, bool &gameRunning, SDL_Event &event, SDL_Window* window)
{
	if (event.type == SDL_QUIT)
	{
		gameRunning = false;
		game = false;
	}

	if (event.key.keysym.sym == SDLK_ESCAPE)
	{
		gameRunning = false;
		game = false;
	}
}

bool collideCenter(SDL_Rect obj, SDL_Rect cobj)
{
	if (cobj.x + cobj.w/2 > obj.x - cobj.w/4 &&
		cobj.y + cobj.h/2 > obj.y - cobj.w/4 &&
		cobj.x + cobj.w/2 < obj.x + obj.w + cobj.w/4 &&
		cobj.y + cobj.h/2 < obj.y + obj.h + cobj.w/4)
	{
		return true;
	}
	else
		return false;
}


int main(int argc, char* args[])
{
	bool gameRunning = true;
	SDL_Event event;

	// screensize
	int winW = 1280;
	int winH = 720;

	// deltaTime, used to make some things not go too fast because you tried to pay to win (ran the program fast)
	Uint64 deltaNow = SDL_GetPerformanceCounter();
	Uint64 deltaLast = 0;
	double deltaTime = 0;

	// init SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	SDL_Window* window = SDL_CreateWindow("Drater", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Surface *icon = IMG_Load("assets/icon.png");
	SDL_SetWindowIcon(window, icon);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_RenderSetLogicalSize(renderer, winW, winH);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	Mix_Music* draterBgMus = Mix_LoadMUS("assets/draterBgMusic.wav");

	int r = 191;
	int g = 122;
	int b = 66;
	SDL_SetRenderDrawColor(renderer, r, g, b, 0);

	SDL_Texture* draterBgTex = IMG_LoadTexture(renderer, "assets/draterBg.png");
	Entity draterBg = Entity(draterBgTex, 128, 72, 0, 0, winW, winH, false);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	bool menu = true;
	bool game = false;
	int fullscreenMode = -1;

	SDL_Texture* draterLightTex = IMG_LoadTexture(renderer, "assets/draterLight.png");
	Entity draterLight = Entity(draterLightTex, 43, 72, winW / 2, 0, winH * (43.0f / 72.0f), winH, false);

	std::vector<WaterPixel> waters;
	float waterSpawningTime = 0;
	int randomWaterSpawningTime = 1;
	float spawnIncreaser = 1;

	SDL_Texture* waterPixelTexture = IMG_LoadTexture(renderer, "assets/waterpixel.png");
	std::vector<WaterPixel> draters;

	int mx = winW/2;
	int my = winH/2;

	// program running
	while (gameRunning)
	{
		// menu loop
		while (menu)
		{
			deltaLast = deltaNow;
			deltaNow = SDL_GetPerformanceCounter();
			deltaTime = (double)(deltaNow - deltaLast) * 100 / (double)SDL_GetPerformanceFrequency();
			while (SDL_PollEvent(&event))
			{
				ifquit(menu, gameRunning, event, window);
				if (event.type == SDL_KEYDOWN)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_f:
						fullscreenMode *= -1;
						break;
					}
				}
			}

			if (!Mix_PlayingMusic())
			{
				Mix_PlayMusic(draterBgMus, -1);
			}

			if (fullscreenMode == -1)
			{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			if (fullscreenMode == 1)
			{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_SHOWN);
			}

			if (keys[SDL_SCANCODE_RETURN])
			{
				for (int i = 0; i < 3; ++i) {
					for (int o = 0; o < 3; ++o) {

						draters.emplace_back(waterPixelTexture, winW / 2 + o * 32, winH / 2 + i * 32, 16, 16);
					}
				}
				menu = false;
				game = true;
				r = 191;
				g = 122;
				b = 66;
			}

			SDL_RenderClear(renderer);

			draterBg.render(renderer);

			SDL_RenderPresent(renderer);
		}

		// game running
		while (game)
		{
			deltaLast = deltaNow;
			deltaNow = SDL_GetPerformanceCounter();
			deltaTime = (double)(deltaNow - deltaLast) / (double)SDL_GetPerformanceFrequency();
			
			// INPUT
			while (SDL_PollEvent(&event))
			{
				ifquit(game, gameRunning, event, window);

			}

			// UPDATE
			SDL_GetMouseState(&mx, &my);

			if (Mix_PlayingMusic())
			{
				Mix_PauseMusic();
			}

			if (spawnIncreaser < 3)
				spawnIncreaser += deltaTime / 10.0f;
			waterSpawningTime += deltaTime * spawnIncreaser;
			if (waterSpawningTime > randomWaterSpawningTime) {
				waterSpawningTime = 0;
				randomWaterSpawningTime = rand() % 4 + 1;
				waters.emplace_back(waterPixelTexture, rand() % (winW - 50) + 25, rand() % (winH - 50) + 25, 16, 16);
			}

			// RENDER
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderClear(renderer);

			draterLight.x -= 50.0f * deltaTime;
			if (draterLight.x < -1000)
				draterLight.x = winW + 1000;
			draterLight.moveUpdate();
			draterLight.render(renderer);

			for (int p = 0; p < waters.size(); ++p)
			{
				waters[p].decrease(deltaTime * 50);
				waters[p].updatePos(deltaTime);
				waters[p].render(renderer);

				if (waters[p].alpha <= 1)
					waters.erase(waters.begin() + p);
			}

			for (int i = 0; i < draters.size(); ++i) {
				#define d draters[i]
				d.damp = 0.1;
				for (int o = 0; o < draters.size(); ++o) {
					#define ot draters[o]
					if (d.s_x < ot.s_x + ot.w && d.s_x + d.w > ot.x && d.s_y < ot.s_y + ot.h && d.s_y + d.h > ot.y){
						d.speedX += rand() % 11 - 5;
						d.speedY += rand() % 11 - 5;
					}
				}

				for (int p = 0; p < waters.size(); ++p){
					if (collideCenter(d.rect, waters[p].rect)) {
						waters[p].alpha = 200;
						draters.push_back(waters[p]);
						waters.erase(waters.begin() + p);
					}
				}
				
				d.noExplore(winW, winH);
				float distanceX = mx - d.x;
				float distanceY = my - d.y;
				float distance = sqrt(distanceX * distanceX + distanceY * distanceY);
				d.speedX += distanceX / distance * deltaTime * 1500;
				d.speedY += distanceY / distance * deltaTime * 1500;
				
				d.decrease(deltaTime * (rand() % 30));
				d.updatePos(deltaTime);
				d.render(renderer);
				
				if (d.alpha <= 1)
					draters.erase(draters.begin() + i);
			}
			Log(draters.size());
			if (draters.size() <= 0) {
				Log("ee");
				waters.clear();
				draters.clear();
				game = false;
				menu = true;
			}

			SDL_RenderPresent(renderer);
			Log(SDL_GetError());
		}
		
	}
	SDL_DestroyWindow(window);
	Mix_FreeMusic(draterBgMus);

	draterBgMus = nullptr;

	Mix_Quit();
	SDL_Quit();


	return 0;
}