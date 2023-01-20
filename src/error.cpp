#include "error.h"

void error(std::string msg)
{
    // log to terminal & show message box
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", msg.c_str());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg.c_str(), nullptr);
    exit(-1);
}
