#include <SDL.h>

class Entity
{
public:
    SDL_Texture* tex;
    SDL_Rect srcRect;
    SDL_Rect rect;
    bool center;
    float x;
    float y;
    float cx;
    float cy;
    float w;
    float h;
    float speedX;
    float speedY;
    float alpha;
    float angle;

    Entity(SDL_Texture* texture, int source_width, int source_height, float position_x, float position_y, int width, int height, bool centered);

    Entity() = default;

    void render(SDL_Renderer* renderer);

    void update();

    void moveUpdate();
};