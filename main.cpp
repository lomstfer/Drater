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

	Mix_Music* cricketsBg = Mix_LoadMUS("assets/cricketsBg.wav");
	Mix_Chunk* ratDieSound = Mix_LoadWAV("assets/ratDie.wav");

	int r = 140;
	int g = 101;
	int b = 73;
	SDL_SetRenderDrawColor(renderer, r, g, b, 0);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	bool menu = true;
	bool game = false;
	int fullscreenMode = -1;


	SDL_Texture* enemyTexture = IMG_LoadTexture(renderer, "assets/rat1.png");
	std::vector<Entity> enemies;


	SDL_Texture* waterPixelTexture = IMG_LoadTexture(renderer, "assets/waterpixel.png");
	Player waterCenter = Player(waterPixelTexture, winW / 2, winH / 2, 32, 32);
	std::vector<WaterPixel> draters;

	for (int i = 0; i < 10; ++i) {
		for (int o = 0; o < 10; ++o) {
			draters.emplace_back(waterPixelTexture, winW / 2 + o * 32 - 32 * 2.5, winH / 2 + i * 32 - 32 * 2.5, 16, 16);
		}
	}
	//WaterPixel pixel1 = WaterPixel();

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

			if (Mix_PlayingMusic())
			{
				Mix_PauseMusic();
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
				menu = false;
				game = true;

				r = 140;
				g = 101;
				b = 73;
			}

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
			if (!Mix_PlayingMusic())
			{
				Mix_PlayMusic(cricketsBg, -1);
			}
			else if (Mix_PausedMusic())
			{
				Mix_ResumeMusic();
			}


			// RENDER
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderClear(renderer);
			/*for (int i = 0; i < enemies.size(); ++i)
			{
				SDL_RenderCopyEx(renderer, enemies[i].tex, &enemies[i].srcRect, &enemies[i].rect, enemies[i].angle, NULL, SDL_FLIP_NONE);
			}*/

			waterCenter.update(winW, winH, deltaTime, keys);
			waterCenter.noExplore(winW, winH);
			waterCenter.render(renderer);

			for (int i = 0; i < draters.size(); ++i) {
				#define d draters[i]
				d.damp = 0.1;
				for (int o = 0; o < draters.size(); ++o) {
					#define ot draters[o]
					if (d.s_x < ot.s_x + ot.w && d.s_x + d.w > ot.x && d.s_y < ot.s_y + ot.h && d.s_y + d.h > ot.y){
						d.speedX += rand() % 5 - 2;
						d.speedY += rand() % 5 - 2;
					}
				}
				
				d.noExplore(winW, winH);
				float distanceX = waterCenter.x - d.x;
				float distanceY = waterCenter.y - d.y;
				float distance = sqrt(distanceX * distanceX + distanceY * distanceY);
				d.speedX += distanceX / distance * deltaTime * (rand() % 500);
				d.speedY += distanceY / distance * deltaTime * (rand() % 500);
				d.updatePos(deltaTime);
				d.render(renderer);

				d.decrease(deltaTime);
				if (d.w <= 0)
					draters.erase(draters.begin() + i);
			}
			SDL_RenderPresent(renderer);
		}
		SDL_DestroyWindow(window);
		Mix_FreeChunk(ratDieSound);
		Mix_FreeMusic(cricketsBg);

		ratDieSound = nullptr;
		cricketsBg = nullptr;

		Mix_Quit();
		SDL_Quit();


		return 0;
	}
}