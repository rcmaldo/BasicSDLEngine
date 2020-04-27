#include "..\headers\Entity.h"

#include "..\headers\Init.h"

/* Hit Test for Circular Entities */
bool Entity::isHitCircle(Entity& e1, Entity& e2)
{
    double center1x = e1.x + e1.getGRect().w / 2;
    double center1y = e1.y + e1.getGRect().h / 2;
    double center2x = e2.x + e2.getGRect().w / 2;
    double center2y = e2.y + e2.getGRect().h / 2;
    double dist = sqrt(pow(center1x - center2x, 2.0) + pow(center1y - center2y, 2.0));
    return dist <= e1.getRadius() + e2.getRadius();
}

/* Regular C-tor */
Entity::Entity(std::string spritePath, int x, int y, int w, int h, double radius)
    : x(gRect.x), y(gRect.y), delta(Game::getDelta())
{
    /* Load sprite or image */
    texture = Game::loadTexture(spritePath);

    int queryW = 0;
    int queryH = 0;

    SDL_QueryTexture(texture, NULL, NULL, &queryW, &queryH);

    lRect.x = 0;
    lRect.y = 0;
    lRect.w = queryW;
    lRect.h = queryH;
    
    /* Set dimensions */
    gRect.x = x;
    gRect.y = y;
    if (w < 0) {
        gRect.w = queryW;
    } else {
        gRect.w = w;
    }
    if (h < 0) {
        gRect.h = queryH;
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

/* Copy C-tor (DOES NOT WORK WITH TRANSPARENCY) */
/* TODO: shared ptr for textures */
Entity::Entity(const Entity& other)
    : x(gRect.x), y(gRect.y), delta(Game::getDelta()), radius(other.radius),
    gRect(other.gRect), lRect(other.lRect)
{
    /* Get width and height, then create blank texture */
    int queryW;
    int queryH;
    SDL_Renderer* gRenderer = Game::getRenderer();
    SDL_QueryTexture(other.texture, NULL, NULL, &queryW, &queryH);
    this->texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, queryW, queryH);
    
    /* Get previous render target */
    SDL_Texture* prevTarget = SDL_GetRenderTarget(gRenderer);
    
    /* Set target to object's texture */
    SDL_SetRenderTarget(gRenderer, this->texture);

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
    SDL_RenderFillRect(gRenderer, NULL);

    /* Copy texture */
    SDL_RenderCopy(gRenderer, other.texture, NULL, NULL);
    
    /* Set back render target */
    SDL_SetRenderTarget(gRenderer, prevTarget);
}

/* Copy A-tor (DOES NOT WORK WITH TRANSPARENCY) */
Entity& Entity::operator=(const Entity& other)
{
    this->radius = other.radius;
    this->gRect = other.gRect;
    this->lRect = other.lRect;

    /* Get width and height, then create blank texture */
    int queryW;
    int queryH;
    SDL_Renderer* gRenderer = Game::getRenderer();
    SDL_QueryTexture(other.texture, NULL, NULL, &queryW, &queryH);
    this->texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, queryW, queryH);
    
    /* Get previous render target */
    SDL_Texture* prevTarget = SDL_GetRenderTarget(gRenderer);
    
    /* Set target to object's texture */
    SDL_SetRenderTarget(gRenderer, this->texture);

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
    SDL_RenderFillRect(gRenderer, NULL);

    /* Copy texture */
    SDL_RenderCopy(gRenderer, other.texture, NULL, NULL);
    
    /* Set back render target */
    SDL_SetRenderTarget(gRenderer, prevTarget);
}

/* Move C-tor */
Entity::Entity(Entity&& other)
    : x(gRect.x), y(gRect.y), delta(Game::getDelta()), radius(other.radius),
    gRect(other.gRect), lRect(other.lRect)
{
    this->texture = other.texture;
    other.texture = NULL;
}

/* Move A-tor */
Entity& Entity::operator=(Entity&& other)
{
    this->radius = other.radius;
    this->gRect = other.gRect;
    this->lRect = other.lRect;

    this->texture = other.texture;
    other.texture = NULL;
}

Entity::~Entity()
{
    SDL_DestroyTexture(texture);
}