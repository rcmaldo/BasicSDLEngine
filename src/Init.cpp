#include "..\headers\Init.h"

Init Init::sInstance;

bool Init::initializeSDL(int width, int height, int fps, std::vector<int>& eventList)
{
    sInstance.width = width;
    sInstance.height = height;
    sInstance.fps = fps;
    sInstance.delta = 1.0f / fps;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        sInstance.log << "SDL2 initialization failed!\nSDL_Error:" << SDL_GetError() << "\n";
        return false;
    }

    /* Set texture filtering to linear */
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        sInstance.log << "Warning: Linear texture filtering not enabled!\n";
    }

    /* Initialize main window */
    sInstance.window = SDL_CreateWindow("TheGame0", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sInstance.width, sInstance.height, SDL_WINDOW_SHOWN);
    if (sInstance.window == NULL) {
        sInstance.log << "Main window failed to initialize!\nSDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    /* Initialize renderer */
    sInstance.renderer = SDL_CreateRenderer(sInstance.window, -1, SDL_RENDERER_ACCELERATED);
    if( sInstance.renderer == NULL )
    {
        sInstance.log << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    /* Initialize renderer color */
    SDL_SetRenderDrawBlendMode(sInstance.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sInstance.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    /* Initialize PNG system from SDL_image */
    int imgFlags = IMG_INIT_PNG;
    if (!( IMG_Init( imgFlags ) & imgFlags )) {
        sInstance.log << "SDL_image failed to initialize!\nSDL_image Error: " << IMG_GetError() << "\n";
        return false;
    }

    /* Set the events for later handling */
    for (int e : eventList) {
        sInstance.events[e] = false;
    }

    return true;
}

void Init::close()
{
    /* Destroy resources */
    SDL_DestroyRenderer(sInstance.renderer);
    SDL_DestroyWindow(sInstance.window);
    sInstance.window = NULL;
    sInstance.renderer = NULL;

    /* Quit SDL subsystems */
	IMG_Quit();
    SDL_Quit();
}

SDL_Texture* Init::loadTexture(std::string path)
{
    SDL_Texture* retTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        sInstance.log << "Unable to load image " << path.c_str() << "! SDL_image Error: " << IMG_GetError() << "\n";
        return NULL;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF) );

    retTexture = SDL_CreateTextureFromSurface(sInstance.renderer, loadedSurface);
    if (retTexture == NULL) {
        sInstance.log << "Unable to create image from " << path.c_str() << "! SDL_image Error: " << IMG_GetError() << "\n";
    }
    SDL_FreeSurface(loadedSurface);
    
    return retTexture;
}

/* Add entity object, make sure entity is rvalue to reduce resource usage */
Entity& Init::addEntity(std::string id, Entity&& entity)
{
    auto p = sInstance.entityList.emplace(id, std::move(entity));
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

void Init::textureHandler()
{
    SDL_SetRenderDrawColor(sInstance.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(sInstance.renderer);
    for (auto &it : sInstance.entityList) {
        /* Apply image onto global surface */
        Entity& e = it.second;
        SDL_Rect src = e.getLRect().getSDLRect();
        SDL_Rect dst = e.getGRect().getSDLRect();
        SDL_RenderCopy(sInstance.renderer, e.getTexture(), &src, &dst);
    }

    /* Update global texture */
    SDL_RenderPresent(sInstance.renderer);
}

void Init::destroyHandler()
{
    while (!sInstance.destroyQueue.empty()) {
        std::string id = sInstance.destroyQueue.front();
        sInstance.destroyQueue.pop();

        sInstance.entityList.erase(id);
    }
}