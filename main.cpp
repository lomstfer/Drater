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
#include "Colortransition.hpp"


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
	SDL_ShowCursor(SDL_DISABLE);
	SDL_RenderSetLogicalSize(renderer, winW, winH);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	Mix_Music* draterBgMus = Mix_LoadMUS("assets/draterBgMusic.wav");
	Mix_Music* draterDayMus = Mix_LoadMUS("assets/draterday.wav");
	Mix_Chunk* waterCatch = Mix_LoadWAV("assets/waterCatch.wav");

	float r = 108.0f;
	float g = 99.0f;
	float b = 128.0f;
	SDL_SetRenderDrawColor(renderer, r, g, b, 0);

	SDL_Texture* draterBgTex = IMG_LoadTexture(renderer, "assets/draterBg.png");
	Entity draterBg = Entity(draterBgTex, 128, 72, 0, 0, winW, winH, false);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	bool menu = true;
	bool game = false;
	int fullscreenMode = -1;

	SDL_Texture* draterLightTex = IMG_LoadTexture(renderer, "assets/draterLight-2.png");
	Entity draterLight = Entity(draterLightTex, 20, 100, winW / 2, -200, 200, 20000, true);
	draterLight.angle = -90.0f;
	draterLight.r = 255;
	draterLight.g = 255;
	draterLight.r = 255;
	// night = -1 day = 1
	int nightDay = -1;
	float days = 0;
	float bestDays = days;

	std::vector<WaterPixel> waters;
	float waterSpawningTime = 0;
	int randomWaterSpawningTime = 1;
	float spawnIncreaser = 1;

	SDL_Texture* waterPixelTexture = IMG_LoadTexture(renderer, "assets/waterpixel.png");
	SDL_Texture* waterPlayerTexture = IMG_LoadTexture(renderer, "assets/waterpixel.png");
	std::vector<WaterPixel> draters;

	int mx = winW/2;
	int my = winH/2;

	Text title = Text("Drater", 100, { 173, 123, 94 }, "assets/Debrosee.ttf", winW - 320, winH / 2 - 182, true, renderer);
	Text instT = Text("enter to start", 60, { 117, 112, 107 }, "assets/Pixellettersf.ttf", 300, winH / 2 - 100, true, renderer);
	Text whathappened = Text("Mouse controls", 60, {250, 250, 250}, "assets/Pixellettersf.ttf", winW / 2, 20, true, renderer);

	double gameTime = 0;


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

						draters.emplace_back(waterPlayerTexture, float(winW) / 2.0f + float(o) * 2.0f, float(winH) / 2.0f + float(i) * 2.0f, 16.0f, 16.0f);
					}
				}
				menu = false;
				game = true;
				nightDay = -1;
				r = 108.0f;
				g = 99.0f;
				b = 128.0f;

				if (Mix_PlayingMusic())
				{
					Mix_HaltMusic();
				}
			}

			SDL_RenderClear(renderer);

			draterBg.render(renderer);
			title.render();
			instT.render();
			whathappened.update();
			whathappened.render();

			SDL_RenderPresent(renderer);
		}

		// game running
		while (game)
		{
			deltaLast = deltaNow;
			deltaNow = SDL_GetPerformanceCounter();
			deltaTime = (double)(deltaNow - deltaLast) / (double)SDL_GetPerformanceFrequency();
			gameTime += deltaTime;
			// INPUT
			while (SDL_PollEvent(&event))
			{
				ifquit(game, gameRunning, event, window);
			}

			// UPDATE
			SDL_GetMouseState(&mx, &my);

			if (nightDay == -1)
				spawnIncreaser = 10.0f;
			if (nightDay == 1)
				spawnIncreaser = 2.0f;
			waterSpawningTime += deltaTime * spawnIncreaser;
			if (waterSpawningTime > randomWaterSpawningTime + days) {
				waterSpawningTime = 0;
				randomWaterSpawningTime = rand() % 3 + 1;
				waters.emplace_back(waterPixelTexture, rand() % (winW - 50) + 25, rand() % (winH - 50) + 25, 16, 16);
			}

			// RENDER
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderClear(renderer);

			draterLight.angle += 5.0f * deltaTime;
			draterLight.w = (360.0f - 5 * fabsf(draterLight.angle));
			//draterLight.h = 72.0f * (draterLight.angle / 10.0f);
			if (draterLight.angle > 90) {
				draterLight.angle = -90.0f;
				nightDay *= -1;
				days += 0.5;
			}

			if (nightDay == -1) {
				if (Mix_PlayingMusic())
				{
					Mix_HaltMusic();
				}
				r = 191.0f / 1.5f;
				g = 122.0f / 1.5f;
				b = 66.0f / 1.5f;
				draterLight.r = r + 10.0f;
				draterLight.g = g + 10.0f;
				draterLight.b = b + 10.0f;
				SDL_SetTextureColorMod(draterLight.tex, draterLight.r, draterLight.g, draterLight.b);
			}

			if (nightDay == 1) {
				if (!Mix_PlayingMusic())
				{
					Mix_PlayMusic(draterDayMus, -1);
				}

				r = 191.0f;
				g = 122.0f;
				b = 66.0f;
				draterLight.r = r + 10.0f;
				draterLight.g = g + 10.0f;
				draterLight.b = b + 10.0f;
				SDL_SetTextureColorMod(draterLight.tex, draterLight.r, draterLight.g, draterLight.b);
			}

			draterLight.moveUpdate();
			draterLight.render(renderer);

			for (int p = 0; p < waters.size(); ++p)
			{
				waters[p].decrease(deltaTime * 75.0f);
				waters[p].updatePos(deltaTime);
				waters[p].render(renderer);

				if (waters[p].alpha <= 10)
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

				d.noExplore(winW, winH);
				float distanceX = mx - d.x;
				float distanceY = my - d.y;
				float distance = sqrt(distanceX * distanceX + distanceY * distanceY);
				d.speedX += distanceX / distance * deltaTime * 1500;
				d.speedY += distanceY / distance * deltaTime * 1500;
				
				if (nightDay == -1)
					d.decrease(deltaTime * (rand() % 200) / draters.size());
				else
					d.decrease(deltaTime * (rand() % 600) / draters.size());

				d.updatePos(deltaTime);
				d.render(renderer);

				for (int y = 0; y < waters.size(); ++y) {
					if (collideCenter(d.rect, waters[y].rect)) {
						Mix_PlayChannel(-1, waterCatch, 0);
						waters[y].alpha = 200;
						waters[y].tex = waterPlayerTexture;
						draters.push_back(waters[y]);
						waters.erase(waters.begin() + y);
					}
				}

				if (d.alpha <= 10)
					draters.erase(draters.begin() + i);
			}

			if (draters.size() <= 0) {
				if (Mix_PlayingMusic())
				{
					Mix_HaltMusic();
				}
				waters.clear();
				draters.clear();
				game = false;
				menu = true;
				if (days > bestDays)
				{
					bestDays = days;
					if (days == 1)
						whathappened.text = "New Highscore: < " + std::to_string(ftint(days)) + " > Day";
					else
						whathappened.text = "New Highscore: < " + std::to_string(ftint(days)) + " > Days";
				}
				if (days <= bestDays)
				{
					if (days == 1)
						whathappened.text = "Highscore: < " + std::to_string(ftint(days)) + " > Day";
					else
						whathappened.text = "Highscore: < " + std::to_string(ftint(days)) + " > Days";
				}
				days = 0;

			}
			SDL_RenderPresent(renderer);
		}
	}
	SDL_DestroyWindow(window);
	Mix_FreeMusic(draterBgMus);
	draterBgMus = nullptr;
	Mix_FreeMusic(draterDayMus);
	draterDayMus = nullptr;
	Mix_FreeChunk(waterCatch);
	waterCatch = nullptr;

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
	Log(SDL_GetError());

	return 0;
}