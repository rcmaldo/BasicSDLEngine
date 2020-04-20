#include "..\headers\Entity.h"

#include "..\headers\Init.h"

/* Regular C-tor */
Entity::Entity(std::string spritePath, int x, int y, int w, int h, float radius)
    : x(gRect.x), y(gRect.y), delta(Init::getDelta())
{
    /* Load sprite or image */
    surface = Init::loadSurface(spritePath);
    lRect.x = 0;
    lRect.y = 0;
    lRect.h = surface->h;
    lRect.w = surface->w;
    
    /* Set dimensions */
    gRect.x = x;
    gRect.y = y;
    if (w < 0) {
        gRect.w = surface->w;
    } else {
        gRect.w = w;
    }
    if (h < 0) {
        gRect.h = surface->h;
    } else {
        gRect.h = h;
    }

    /* Set radius */
    if (radius < 0) {
        this->radius = gRect.w / 2.0f;
    } else {
        this->radius = radius;
    }
}

/* Copy C-tor */
Entity::Entity(const Entity& other)
    : x(gRect.x), y(gRect.y), delta(Init::getDelta()), radius(other.radius),
    gRect(other.gRect), lRect(other.lRect)
{
    this->surface = SDL_ConvertSurface(other.surface, other.surface->format, SDL_SWSURFACE);
}

/* Copy A-tor */
Entity& Entity::operator=(const Entity& other)
{
    this->radius = other.radius;
    this->gRect = other.gRect;
    this->lRect = other.lRect;
    
    this->surface = SDL_ConvertSurface(other.surface, other.surface->format, SDL_SWSURFACE);
}

/* Move C-tor */
Entity::Entity(Entity&& other)
    : x(gRect.x), y(gRect.y), delta(Init::getDelta()), radius(other.radius),
    gRect(other.gRect), lRect(other.lRect)
{
    this->surface = other.surface;
    other.surface = NULL;
}

/* Move A-tor */
Entity& Entity::operator=(Entity&& other)
{
    this->radius = other.radius;
    this->gRect = other.gRect;
    this->lRect = other.lRect;

    this->surface = other.surface;
    other.surface = NULL;
}

Entity::~Entity()
{
    SDL_FreeSurface(surface);
}