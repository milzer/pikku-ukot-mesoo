#include "Diesel.h"

#include <algorithm>
#include <filesystem>

#include <SDL3/SDL.h>

// The game advances some effects per frame rather than per second, so it runs
// at the refresh rate of the 320x200 VGA mode it was made for.
static const Uint64 s_FrameRate = 70;

IDieselApplication::IDieselApplication()
    : m_hwnd(NULL), m_fFrameTime(0.0f), m_dwTimeGetTimeVal(0),
      m_pRenderer(NULL), m_pTexture(NULL), m_pcszTitle(""), m_bRunning(false), m_fFps(0.0f)
{
}

IDieselApplication::~IDieselApplication()
{
}

void IDieselApplication::SetAppTitle(LPCTSTR pcszTitle)
{
    m_pcszTitle = pcszTitle;
}

void IDieselApplication::SetResources(DWORD dwMenu, DWORD dwIcon, DWORD dwAccel)
{
}

void IDieselApplication::LockWindowSize(BOOL bLock)
{
    SDL_SetWindowResizable(m_hwnd, !bLock);
}

DE_RETVAL IDieselApplication::Startup(void* pParent, SDE_DISPLAYMODE* psMode, DWORD dwFlags)
{
    // Data files are opened relative to the executable, as on Windows
    std::filesystem::current_path(std::filesystem::u8path(SDL_GetBasePath()));

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return DE_FAILED;
    }

    // Open the window at the largest whole multiple of the mode that fits the desktop
    SDL_Rect usable;
    int scale = 1;
    if (SDL_GetDisplayUsableBounds(SDL_GetPrimaryDisplay(), &usable))
    {
        scale = std::max(1, std::min(usable.w / psMode->iWidth, usable.h / psMode->iHeight) - 1);
    }

    if (!SDL_CreateWindowAndRenderer(m_pcszTitle, psMode->iWidth * scale, psMode->iHeight * scale,
                                     SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY, &m_hwnd, &m_pRenderer))
    {
        return DE_FAILED;
    }
    SDL_SetRenderLogicalPresentation(m_pRenderer, psMode->iWidth, psMode->iHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING,
                                   psMode->iWidth, psMode->iHeight);
    if (!m_pTexture)
    {
        return DE_FAILED;
    }
    SDL_SetTextureScaleMode(m_pTexture, SDL_SCALEMODE_PIXELART);
    SDL_HideCursor();

    SDE_SURFACEDESC desc = {DE_SYSTEMMEMORY, eDE_COMPATIBLE, psMode->iWidth, psMode->iHeight};
    m_srfBack.Create(desc);

    return OnInitDone();
}

int IDieselApplication::Run()
{
    const Uint64 frameNs = SDL_NS_PER_SECOND / s_FrameRate;
    Uint64 last = SDL_GetTicksNS();
    Uint64 next = last;

    m_bRunning = true;

    while (m_bRunning)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                m_bRunning = false;
            }
            else if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_RETURN && (e.key.mod & SDL_KMOD_ALT))
            {
                SDL_SetWindowFullscreen(m_hwnd, !(SDL_GetWindowFlags(m_hwnd) & SDL_WINDOW_FULLSCREEN));
            }
        }
        if (!m_bRunning)
        {
            break;
        }

        Uint64 now = SDL_GetTicksNS();
        m_fFrameTime = (float)(now - last) / SDL_NS_PER_SECOND;
        m_dwTimeGetTimeVal = (DWORD)SDL_NS_TO_MS(now);
        last = now;

        if (m_fFrameTime > 0.0f)
        {
            m_fFps += (1.0f / m_fFrameTime - m_fFps) * 0.1f;
        }

        OnFlip();

        SDL_UpdateTexture(m_pTexture, NULL, m_srfBack.Lock(NULL), m_srfBack.GetWidth() * sizeof(DWORD));
        m_srfBack.Unlock();
        SDL_RenderClear(m_pRenderer);
        SDL_RenderTexture(m_pRenderer, m_pTexture, NULL, NULL);
        SDL_RenderPresent(m_pRenderer);

        next += frameNs;
        now = SDL_GetTicksNS();
        if (next > now)
        {
            SDL_DelayPrecise(next - now);
        }
        else
        {
            next = now;
        }
    }

    OnExit();

    m_srfBack.Release();
    SDL_DestroyTexture(m_pTexture);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_hwnd);
    SDL_Quit();

    return 0;
}

void IDieselApplication::Shutdown()
{
    m_bRunning = false;
}

float IDieselApplication::GetFps() const
{
    return m_fFps;
}
