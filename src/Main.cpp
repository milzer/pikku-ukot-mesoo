/*--------------------------------------------------------------------------------------
  Main.cpp source file
  Generated by Diesel Application wizard (c) 2001 Inmar Software Ltd.

  Date: 8.6.2001

  pumApp Application Winmain function and DieselEngine Initialization

--------------------------------------------------------------------------------------*/

#include "pumApp.h"
#include "resource.h"


CpumApp theApp;


int PASCAL WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   TCHAR *pCmdLine,
                   int nCmdShow)
{
    // Initialize DieselEngine
    {
        DE_RETVAL       res = DE_OK;

        theApp.SetAppTitle(_T("pum"));

        // Set Application resource IDs here
        theApp.SetResources(0, IDI_APPICON, 0);

        SDE_DISPLAYMODE     mode;
        mode.iWidth = 320;
        mode.iHeight = 200;
        mode.iRefresh = 0;
        mode.iBPP = 32;

        DWORD dwStartupFlags =  0;
//      dwStartupFlags |= DE_CREATEZBUFFER;
//      dwStartupFlags |= DE_WINDOWED;

        // Startup the engine. If derived class OnInitDone function failed,
        // return value is passed here
        res = theApp.Startup(NULL, &mode, dwStartupFlags);
        if (res != DE_OK) {
            ::MessageBox(NULL,
                         _T("Failed to start Application"),
                         _T("Fatal Error"),
                         MB_ICONHAND | MB_OK | MB_DEFBUTTON1);
            return 0;
        }

        theApp.LockWindowSize(FALSE);
    }

    // start pumping messages
    int ret = 0;
    ret = theApp.Run();
    return ret;
}
