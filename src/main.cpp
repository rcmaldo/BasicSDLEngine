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

int WinMain(int argc, char *argv[])
{
    if (!Init::initializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FRAMES_PER_SECOND, eventList)) {
        printf("Initialization failed:\n");
    } else {

        bool quit = false;

        SDL_Event e;

        int i = 0;

        Entity& player = Init::addEntity("15_player", Entity("Player.png"));
        player.update = [&]() {
            const Uint8* currKeystates = SDL_GetKeyboardState( NULL );

            if (player.y < SCREEN_HEIGHT - player.getGRect().h) {
                player.y += 25 * Init::getDelta();
            }

            if (currKeystates[SDL_SCANCODE_UP]) {
                player.y -= 50 * Init::getDelta();
            }

            if (currKeystates[SDL_SCANCODE_LEFT]) {
                player.x -= 25 * Init::getDelta();
            }

            if (currKeystates[SDL_SCANCODE_RIGHT]) {
                player.x += 25 * Init::getDelta();
            }

            if (Init::getEvent(SDLK_SPACE)) {
                i++;
                std::string id = "11_pBullet";
                id += std::to_string(i);
                Entity& bullet = Init::addEntity(id, Entity("pBullet.png", player.x + player.getRadius() - 15, player.y));
                bullet.update = [&, id]() {
                    bullet.y -= 10 * Init::getDelta();
                    if (bullet.y < SCREEN_HEIGHT - 10) {
                        Init::destroyEntity(id);
                    }
                };
            }
        };

        Uint32 timeCap = 1000 / Init::getFPS();
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

            Init::eventHandler();
            Init::updateHandler();
            Init::textureHandler();
            // TODO Init::addHandler(); REQUIRED (add Entities spawned by other Entities during update())
            Init::destroyHandler();

            if (Init::getEvent(SDL_QUIT) || Init::getEvent(SDLK_a)) {
                quit = true;
            }

            // if less than 16ms, delay 
            if (actualDelta < timeCap) {
                SDL_Delay(timeCap - actualDelta);
            }
            
            // if delta is bigger than 16ms between frames, get the actual fps
            int fps = Init::getFPS();
            if (actualDelta > 0) {
                fps = 1000 / actualDelta;
            }
            
            std::string title = "FPS is: ";
            title += std::to_string(fps);
            
            startTime = endTime;
            endTime = SDL_GetTicks();
            SDL_SetWindowTitle(Init::getWindow(), title.c_str());
        }
    }

    /* Close all */
    Init::close();

    return 0;
}