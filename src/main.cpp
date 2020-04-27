#include <cstdio>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "..\headers\Init.h"
#include "..\headers\Entity.h"

/* Window dimensions */
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 640;
const int FRAMES_PER_SECOND = 60;

std::vector<int> eventList = {SDL_QUIT, SDLK_a, SDLK_SPACE};

int enemyCount = 0;
int bossHP = 15;
std::map<std::string, Entity*> enemies = {};
Entity* addRegEnemy(int x = 0)
{
    std::string id = "12_enemy";
    id += std::to_string(++enemyCount);
    Entity* self = Game::addEntity(id, Entity("Enemy.png", x));
    self->y = -self->getGRect().h;
    Entity* player = Game::findEntity("15_player");

    enemies.emplace(id,self);

    self->update = [&, self, player]() {
        self->y += 50 * Game::getDelta();
        if (self != NULL && player != NULL && Entity::isHitCircle(*self, *player)) {
            Game::destroyEntity("15_player");
        }
    };
}


Entity* addLargeEnemy(int x = 0)
{
    std::string id = "14_enemy";
    id += std::to_string(++enemyCount);
    Entity* self = Game::addEntity(id, Entity("Enemy.png", x));
    self->y = -self->getGRect().h;
    Entity* player = Game::findEntity("15_player");
    
    self->getGRect().w *= 3;
    self->getGRect().h *= 3;

    self->update = [&, self, player, id]() {
        self->y += 20 * Game::getDelta();
        if (Entity::isHitCircle(*self, *player)) {
            Game::destroyEntity("15_player");
        }
        if (bossHP <= 0) {
            Game::destroyEntity(id);
        }
    };

    return self;
}

int WinMain(int argc, char *argv[])
{
    Entity* large = NULL;
    if (!Game::initializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FRAMES_PER_SECOND, eventList)) {
        printf("Initialization failed:\n");
    } else {

        bool quit = false;

        SDL_Event e;

        int i = 0;

        Entity& background = *Game::addEntity("00_background", Entity("Waves.png"));
        Game::tileEntityTexture(background);
        background.update = [&]() {
            background.y += 1;
            if (background.y >= 0) {
                background.y = -50;
            }
        };

        Entity& player = *Game::addEntity("15_player", Entity("Player.png"));
        player.y = SCREEN_HEIGHT - player.getGRect().h;
        player.update = [&]() {
            const Uint8* currKeystates = SDL_GetKeyboardState( NULL );

            if (currKeystates[SDL_SCANCODE_UP]) {
                player.y -= 50 * Game::getDelta();
            }
            
            if (currKeystates[SDL_SCANCODE_DOWN]) {
                player.y += 50 * Game::getDelta();
            }

            if (currKeystates[SDL_SCANCODE_LEFT]) {
                player.x -= 150 * Game::getDelta();
            }

            if (currKeystates[SDL_SCANCODE_RIGHT]) {
                player.x += 150 * Game::getDelta();
            }

            if (Game::getEvent(SDLK_SPACE)) {
                ++i;
                std::string id = "11_pBullet";
                id += std::to_string(i);
                Entity* self = Game::addEntity(id, Entity("pBullet.png", 0, player.y + player.getRadius()));
                self->x = player.x + player.getRadius() - self->getRadius();
                self->update = [&, id, self]() {
                    self->y -= 250 * Game::getDelta();
                    if (self->y < 0) {
                        Game::destroyEntity(id);
                    }

                    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
                        if (self != NULL && it->second != NULL && Entity::isHitCircle(*self, *(it->second))) {
                            Game::destroyEntity(it->first);
                            Game::destroyEntity(id);
                            enemies.erase(it);
                            break;
                        }
                    }

                    if (self != NULL && large != NULL && Entity::isHitCircle(*self, *(large))) {
                        Game::destroyEntity(id);
                        --bossHP;
                    }
                };
            }
        };

        Uint64 frameCount = 0;

        Uint32 timeCap = 1000 / Game::getFPS();
        Uint32 startTime = 0;
        Uint32 endTime = 0;
        Uint32 actualDelta = 0;

        while (!quit) {

            if (!startTime) {
                // get the time in ms passed from the moment the program started
                startTime = SDL_GetTicks(); 
            } else {
                actualDelta = endTime - startTime; // how many ms for a frame
            }

            Game::entityListHandler();
            Game::eventHandler();
            Game::updateHandler();
            Game::textureHandler();
            Game::destroyHandler();

            /* Manual game updates handled below */

            if (Game::getEvent(SDL_QUIT)) {
                quit = true;
            }

            if (frameCount == 1500) {
                large = addLargeEnemy();
                large->x = (Game::getWidth() - large->getGRect().w) / 2;
                large->getRadius() = large->getGRect().w / 2;
            } else if (frameCount < 1100) {
                if (frameCount % 500 == 0) {
                    addRegEnemy();
                    addRegEnemy(Game::getWidth() / 2 - 37);
                    addRegEnemy(Game::getWidth() - 75);
                }

                if (frameCount % 500 == 250) {
                    addRegEnemy();
                    addRegEnemy((Game::getWidth() - 75) / 3 * 1);
                    addRegEnemy((Game::getWidth() - 75) / 3 * 2);
                    addRegEnemy(Game::getWidth() - 75);
                }

            }


            if (bossHP <= 0) {
                large = NULL;
            }
            
            ++frameCount;
            
            /* Manual game updates handled above */

            // if less than 16ms, delay 
            if (actualDelta < timeCap) {
                SDL_Delay(timeCap - actualDelta);
            }
            
            // if delta is bigger than 16ms between frames, get the actual fps
            int fps = Game::getFPS();
            if (actualDelta > timeCap) {
                fps = 1000 / actualDelta;
            }
            
            std::string title = "FPS is: ";
            title += std::to_string(fps);
            
            startTime = endTime;
            endTime = SDL_GetTicks();
            SDL_SetWindowTitle(Game::getWindow(), title.c_str());
        }
    }

    /* Close all */
    Game::close();

    return 0;
}