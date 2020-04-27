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
 
class Game {
public:
    /* Disallow copy constructor */
    Game(const Game&) = delete;

    /* SDL initialization and cleanup calls */
    static bool initializeSDL(int width, int height, int fps, std::vector<int>& eventList);
    static void close();

    /* Load surfaces that are uniform in format to screenSurface */
    static SDL_Surface* loadSurface(std::string path);

    static SDL_Texture* loadTexture(std::string path);

    /* Tiles Entity's current texture to fill map (and +1 row) */
    static void tileEntityTexture(Entity& e);

    /* Handlers */
    static void entityListHandler();
    static void eventHandler();
    static void updateHandler();
    static void textureHandler();
    static void destroyHandler();

    /* Getters */
    static int getWidth() { return sInstance.width; }
    static int getHeight() { return sInstance.height; }
    static int getFPS() { return sInstance.fps; }
    static double& getDelta() { return sInstance.delta; }
    static SDL_Window* getWindow() { return sInstance.window; }
    static SDL_Renderer* getRenderer() { return sInstance.renderer; }
    static std::string getLog() { return sInstance.log.str(); }
    static bool getEvent(int sdlEventName) { return sInstance.events[sdlEventName]; }

    /* Entity functions */
    static Entity* addEntity(std::string id, Entity&& entity);  // Force rvalue reference
    static Entity* findEntity(std::string id);                // Return ptr
    static void destroyEntity(std::string id);                // Add entity to list to be destroyed

private:
    static Game sInstance;

    int width, height;              // Screen dimensions
    std::stringstream log;          // Message/error logs

    int fps;                        // Frames per second
    double delta;                    // := 1/fps
    SDL_Window* window = NULL;      // Global window object
    SDL_Renderer* renderer = NULL;  // Global renderer
    SDL_Texture* texture = NULL;    // Current displayed texture   

    std::map<std::string, Entity> entityList;   // (REMOVE FOR BOILERPLATE)
    std::map<std::string, Entity> addEntityList;// Entities to be added at the end of the cycle
    std::queue<std::string> destroyQueue;       // List of entities to be destroyed

    std::unordered_map<int, bool> events;   // List of SDL_Events, true if active on current frame
                                            // Events are added in initialization
    Game() {};
};

#endif