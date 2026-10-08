#include <SDL3/SDL_main.h>
#include <SDL3/SDL_messagebox.h>

#include "pumApp.h"
#include "resource.h"


CpumApp theApp;


int main(int argc, char* argv[])
{
    {
        DE_RETVAL res = DE_OK;

        theApp.SetAppTitle(_T("pum"));

        theApp.SetResources(0, IDI_APPICON, 0);

        SDE_DISPLAYMODE mode;
        mode.iWidth = 320;
        mode.iHeight = 200;
        mode.iRefresh = 0;
        mode.iBPP = 32;

        DWORD dwStartupFlags = 0;

        res = theApp.Startup(NULL, &mode, dwStartupFlags);
        if (res != DE_OK)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     _T("Fatal Error"),
                                     _T("Failed to start Application"),
                                     NULL);
            return 0;
        }

        theApp.LockWindowSize(FALSE);
    }

    int ret = 0;
    ret = theApp.Run();
    return ret;
}
