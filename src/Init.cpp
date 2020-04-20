#include "..\headers\Init.h"

Init Init::sInstance;

bool Init::initializeSDL(int width, int height, int fps, std::vector<int>& eventList)
{
    sInstance.width = width;
    sInstance.height = height;
    sInstance.fps = fps;
    sInstance.delta = 1.0f / fps;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        sInstance.log << "SDL2 initialization failed!\nSDL_Error:" << SDL_GetError() << "\n";
        return false;
    }

    /* Initialize main window */
    sInstance.window = SDL_CreateWindow("TheGame0", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sInstance.width, sInstance.height, SDL_WINDOW_SHOWN);
    if (sInstance.window == NULL) {
        sInstance.log << "Main window failed to initialize!\nSDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    /* Initialize PNG system from SDL_image */
    int imgFlags = IMG_INIT_PNG;
    if (!( IMG_Init( imgFlags ) & imgFlags )) {
        sInstance.log << "SDL_image failed to initialize!\nSDL_image Error: " << IMG_GetError() << "\n";
        return false;
    }
    
    /* Get window surface */
    sInstance.screenSurface = SDL_GetWindowSurface(sInstance.window);

    /* Set the events for later handling */
    for (int e : eventList) {
        sInstance.events[e] = false;
    }

    return true;
}

void Init::close()
{
    /* Destroy resources */
    SDL_DestroyWindow(sInstance.window);
    sInstance.window = NULL;
    SDL_FreeSurface(sInstance.screenSurface);
    sInstance.screenSurface = NULL;

    /* Quit SDL subsystems */
	IMG_Quit();
    SDL_Quit();
}

SDL_Surface* Init::loadSurface(std::string path)
{
    /* Final output */
    SDL_Surface* retSurface = NULL;

    /* Load image normally */
    SDL_Surface* tmpSurface = IMG_Load(path.c_str());
    if (tmpSurface == NULL) {
        auto err = IMG_GetError();
        printf("IMG_Load failed on %s!\nSDL_image Error: %s\n", path.c_str(), err);
    } else {
        /* Convert image to global surface's format */
        retSurface = SDL_ConvertSurface(tmpSurface, getScreenSurface()->format, 0);
        if (retSurface == NULL) {
            printf("SDL_ConvertSurface failed on %s!\nSDL Error: %s\n", path.c_str(), SDL_GetError());
        }
    }

    /* Dealloc normally loaded surface */
    SDL_FreeSurface(tmpSurface);

    return retSurface;
}

/* Add entity object, make sure entity is rvalue to reduce resource usage */
Entity& Init::addEntity(std::string id, Entity entity)
{
    auto p = sInstance.entityList.emplace(id, entity);
    return (p.first)->second;   // (p.first == the iterator to the pair added) -> Entity
}

/* Finds and returns Entity ptr; nullptr if not found */
Entity* Init::findEntity(std::string id)
{
    auto it = sInstance.entityList.find(id);
    if (it == sInstance.entityList.end()) {
        return nullptr;
    } else {
        return &(it->second);
    }
}

/* Add entity to list, to be destroyed later */
void Init::destroyEntity(std::string id) {
    sInstance.destroyQueue.push(id);
}

/* Event handler */
void Init::eventHandler()
{
    for (auto &it : sInstance.events) {
        it.second = false;
    }

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_KEYDOWN) {
            auto it = sInstance.events.find(e.key.keysym.sym);
            if (it != sInstance.events.end()) {
                it->second = true;
            }
        } else {
            auto it = sInstance.events.find(e.type);
            if (it != sInstance.events.end()) {
                it->second = true;
            }
        }
    }
}

void Init::updateHandler()
{
    /* Call updates on each Entity */
    for (auto &it : sInstance.entityList) {
        it.second.update();
    }
}

void Init::surfaceHandler()
{
    for (auto &it : sInstance.entityList) {
        /* Apply image onto global surface */
        Entity& e = it.second;
        SDL_BlitSurface(e.getSurface(), &(e.getLRect()), Init::getScreenSurface(), &(e.getGRect()));
    }
    
    /* Update global surface */
    SDL_UpdateWindowSurface(Init::getWindow());
}

void Init::destroyHandler()
{
    while (!sInstance.destroyQueue.empty()) {
        std::string id = sInstance.destroyQueue.front();
        sInstance.destroyQueue.pop();

        sInstance.entityList.erase(id);
    }
}