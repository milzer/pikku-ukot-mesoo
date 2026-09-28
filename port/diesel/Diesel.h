#ifndef __DIESEL_H__
#define __DIESEL_H__

#include "DieselTypes.h"
#include "DieselSurface.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

struct SDE_DISPLAYMODE
{
    LONG iWidth;
    LONG iHeight;
    LONG iRefresh;
    LONG iBPP;
};

// Application framework: owns the window and the main loop. Each frame it calls
// OnFlip(), then presents m_srfBack.
class IDieselApplication
{
public:
    IDieselApplication();
    virtual ~IDieselApplication();

    void SetAppTitle(LPCTSTR pcszTitle);
    void SetResources(DWORD dwMenu, DWORD dwIcon, DWORD dwAccel);
    void LockWindowSize(BOOL bLock);

    DE_RETVAL Startup(void* pParent, SDE_DISPLAYMODE* psMode, DWORD dwFlags);
    int Run();
    void Shutdown();

    float GetFps() const;

    virtual DE_RETVAL OnInitDone()
    {
        return DE_OK;
    }
    virtual void OnFlip() = 0;
    virtual void OnExit() {}

protected:
    SDL_Window* m_hwnd;
    CDieselSurface m_srfBack;

    // Duration of the previous frame in seconds
    float m_fFrameTime;
    // Milliseconds since startup
    DWORD m_dwTimeGetTimeVal;

private:
    SDL_Renderer* m_pRenderer;
    SDL_Texture* m_pTexture;
    const char* m_pcszTitle;
    bool m_bRunning;
    float m_fFps;
};

#endif
