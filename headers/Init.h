#ifndef INIT_H
#define INIT_H

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>

#include <SDL.h>
#include <SDL_image.h>

#include "..\headers\Entity.h"
 
class Init {
public:
    /* Disallow copy constructor */
    Init(const Init&) = delete;

    /* SDL initialization and cleanup calls */
    static bool initializeSDL(int width, int height, int fps, std::vector<int>& eventList);
    static void close();

    /* Load surfaces that are uniform in format to screenSurface */
    static SDL_Surface* loadSurface(std::string path);

    /* Handlers */
    static void eventHandler();
    static void updateHandler();
    static void surfaceHandler();
    static void destroyHandler();

    /* Getters */
    static int getWidth() { return sInstance.width; }
    static int getHeight() { return sInstance.height; }
    static int getFPS() { return sInstance.fps; }
    static float& getDelta() { return sInstance.delta; }
    static SDL_Window* getWindow() { return sInstance.window; }
    static SDL_Surface* getScreenSurface() { return sInstance.screenSurface; }
    static std::string getLog() { return sInstance.log.str(); }
    static bool getEvent(int sdlEventName) { return sInstance.events[sdlEventName]; }

    /* Entity functions */
    static Entity& addEntity(std::string id, Entity entity);  // Force rvalue reference
    static Entity* findEntity(std::string id);                // Return ptr
    static void destroyEntity(std::string id);                // Add entity to list to be destroyed

private:
    static Init sInstance;

    int width, height;          // Screen dimensions
    std::stringstream log;      // Message/error logs

    int fps;                    // Frames per second
    float delta;                // := 1/fps
    SDL_Window* window;         // Global window object
    SDL_Surface* screenSurface; // Global main surface object

    std::map<std::string, Entity> entityList;   // (REMOVE FOR BOILERPLATE)
    std::queue<std::string> destroyQueue;       // List of entities to be destroyed

    std::unordered_map<int, bool> events;   // List of SDL_Events, true if active on current frame
                                            // Events are added in initialization
    Init() {};
};

#endif