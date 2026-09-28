#ifndef __DIESELINPUT_H__
#define __DIESELINPUT_H__

#include "DieselInputKeyboard.h"
#include "DieselInputMouse.h"

struct SDL_Window;

class CDieselInput
{
public:
    DE_RETVAL Startup(SDL_Window* hwnd);
    void Shutdown();
};

#endif
