#include <cstdio>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "..\headers\Init.h"
#include "..\headers\Entity.h"

/* Window dimensions */
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 640;

std::vector<int> eventList = {SDL_QUIT, SDLK_a};

int WinMain(int argc, char *argv[])
{
    if (!Init::initializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, 60, eventList)) {
        printf("Initialization failed:\n");
    } else {

        bool quit = false;

        SDL_Event e;

        Entity& ent = Init::addEntity("01_blueCircle", Entity("blue_circle.png"));
        ent.update = [&]() {
            ent.y++;
            if (ent.y > SCREEN_HEIGHT / 2) Init::destroyEntity("01_blueCircle");
        };

        while (!quit) {

            Init::eventHandler();
            Init::updateHandler();
            Init::surfaceHandler();
            Init::destroyHandler();

            if (Init::getEvent(SDL_QUIT) || Init::getEvent(SDLK_a)) {
                quit = true;
            }
        }
    }

    /* Close all */
    Init::close();

    return 0;
}