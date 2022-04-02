#include "Text.hpp"

Text::Text(std::string text, int size, SDL_Color color, std::string fontpath, int x, int y, bool centered, SDL_Renderer* renderer)
	: text(text), size(size), color(color), fontpath(fontpath), x(x), y(y), renderer(renderer), centered(centered)
{
	font = TTF_OpenFont(fontpath.c_str(), size);
	textSurf = TTF_RenderText_Solid(font, text.c_str(), color);
	textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
	SDL_FreeSurface(textSurf);
	SDL_QueryTexture(textTex, nullptr, nullptr, &textRect.w, &textRect.h);
	if (centered)
		textRect.x = x - textRect.w / 2;
	else
		textRect.x = x;
	textRect.y = y;
}

void Text::render()
{
	SDL_RenderCopy(renderer, textTex, nullptr, &textRect);
}

void Text::update()
{
	font = TTF_OpenFont(fontpath.c_str(), size);
	textSurf = TTF_RenderText_Solid(font, text.c_str(), color);
	textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
	SDL_FreeSurface(textSurf);
	SDL_QueryTexture(textTex, nullptr, nullptr, &textRect.w, &textRect.h);
	if (centered)
		textRect.x = x - textRect.w / 2;
	else
		textRect.x = x;
	textRect.y = y;
}
