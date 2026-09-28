#ifndef __DIESELINPUTMOUSE_H__
#define __DIESELINPUTMOUSE_H__

#include "DieselTypes.h"

class CDieselInput;

class CDieselInputMouse
{
public:
    DE_RETVAL Startup(CDieselInput* pInput, DWORD dwFlags);
    void Shutdown();
};

#endif
