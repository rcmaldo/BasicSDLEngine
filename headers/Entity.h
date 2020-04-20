#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include<functional>

#include <SDL.h>
#include <SDL_image.h>

class Entity {
public:
    /* Circular hit collision; true if colliding */
    static bool isHitCircle(Entity& e1, Entity& e2);

    int &x, &y;  // Position, lref to rect object
    
    /* Update function variable */
    std::function<void()> update = [](){}; // Initialize as empty function

    /* C/A/D-tors */
    Entity(std::string spritePath, int x = 0, int y = 0, int w = -1, int h = -1, float radius = -1);
    Entity(const Entity& other);
    Entity& operator=(const Entity& other);
    Entity(Entity&& other);
    Entity& operator=(Entity&& other);
    ~Entity();

    /* Getters */
    SDL_Rect& getGRect() { return gRect; }
    SDL_Rect& getLRect()  { return lRect; }
    SDL_Surface* getSurface()  { return surface; }

protected:
    float &delta;   // := 1/fps, taken from static Init::delta
    float radius;   // radius; generally half-width or half-height

    SDL_Rect gRect;         // Global Dimensions
    SDL_Rect lRect;         // Local Dimensions
    SDL_Surface* surface;   // Sprite image


};

#endif