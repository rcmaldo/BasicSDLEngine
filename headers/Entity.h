#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <functional>
#include <cmath>

#include <SDL.h>
#include <SDL_image.h>

struct fRect {
    double x, y, w, h;
    SDL_Rect getSDLRect() { return {int(round(x)), int(round(y)), int(round(w)), int(round(h))}; }
};

class Entity {
public:
    /* Circular hit collision; true if colliding */
    static bool isHitCircle(Entity& e1, Entity& e2);

    double &x, &y;  // Position, lref to rect object
    
    /* Update function variable */
    std::function<void()> update = [](){}; // Initialize as empty function

    /* C/A/D-tors */
    Entity(std::string spritePath, int x = 0, int y = 0, int w = -1, int h = -1, double radius = -1);
    Entity(const Entity& other);
    Entity& operator=(const Entity& other);
    Entity(Entity&& other);
    Entity& operator=(Entity&& other);
    ~Entity();

    /* Getters */
    fRect& getGRect() { return gRect; }
    fRect& getLRect()  { return lRect; }
    double getDelta() { return delta; }
    double getRadius() { return radius; }
    SDL_Texture* getTexture() { return texture; }

protected:
    double &delta;   // := 1/fps, taken from static Init::delta
    double radius;   // radius; generally half-width or half-height

    fRect gRect;         // Global Dimensions
    fRect lRect;         // Local Dimensions
    SDL_Texture* texture;

};

#endif