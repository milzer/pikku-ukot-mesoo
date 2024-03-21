// pumApp.cpp: implementation of the CpumApp class.
//
//////////////////////////////////////////////////////////////////////

#include "pumApp.h"
#include "io.h"
#include "resource.h"

float globalMatrixTime = 1.0f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CpumApp::CpumApp()
{

}

CpumApp::~CpumApp()
{

}


// OnInitDone()
// Called by DieselEngine, when all initialization is done
// if return value is FAILED, engine will clean up and close
DE_RETVAL CpumApp::OnInitDone()
{
    DE_RETVAL res = DE_OK;
    SDE_SURFACEDESC tmp;

    lv = 0;

    res = m_Input.Startup(m_hwnd);
    if (res != DE_OK)
    {
        return res;
    }

    res = m_Keyboard.Startup(NULL, 0);
    if (res != DE_OK)
    {
        return res;
    }

    res = m_Mouse.Startup(NULL, 0);
    if (res != DE_OK)
    {
        return res;
    }

    CompileLevels();

    res = InitGame(&m_GlobalOptions);
    if (res != DE_OK)
    {
        return res;
    }

    tmp.dwFlags = DE_SYSTEMMEMORY;
    tmp.eFormat = eDE_COMPATIBLE;
    tmp.iWidth = 640;
    tmp.iHeight = 400;

    res = m_Menuback.LoadFromResource(IDR_MENUBG_JPG, _T("JPG"), &tmp);
    if (res != DE_OK)
    {
        return res;
    }

    tmp.iWidth = 320;
    tmp.iHeight = 200;

    res = m_Backbuffer.Create(tmp);
    if (res != DE_OK)
    {
        return res;
    }

    res = m_Backbuffer2.Create(tmp);
    if (res != DE_OK)
    {
        return res;
    }

    SetParticlePalette();
    InitSinCos();

    return DE_OK;
}

int CpumApp::InitGame(SDE_PUMOPTIONS* opt)
{
    LoadOptions(&m_GlobalOptions);
    srand((unsigned)time(NULL));

    GetLevels();

    if (m_ParticleSystem)
    {
        delete m_ParticleSystem;
    }
    m_ParticleSystem = new CPumParticleSystem(m_GlobalOptions.Particles);
    if (!m_ParticleSystem)
    {
        return (DE_OUTOFMEMORY);
    }

    if (m_SoundSystem)
    {
        delete m_SoundSystem;
    }
    m_SoundSystem = new CPumSoundSystem(&m_GlobalOptions);
    if (!m_SoundSystem)
    {
        return (DE_OUTOFMEMORY);
    }
    m_SoundSystem->LoadSoundScript("aeaenet.puo");
    m_SoundSystem->GetMusics();

    if (m_WeaponSystem)
    {
        delete m_WeaponSystem;
    }
    m_WeaponSystem = new CPumWeaponSystem(m_GlobalOptions.Bullets);
    if (!m_WeaponSystem)
    {
        return (DE_OUTOFMEMORY);
    }
    m_WeaponSystem->LoadWeapons();

    if (m_Player)
    {
        delete[] m_Player;
    }
    m_Player = new CPumPlayer[3];
    if (!m_Player)
    {
        return (DE_OUTOFMEMORY);
    }

    for (int i = 0; i < 3; i++)
    {
        m_Player[i].SetColor((rand() & 255) << 16 | (rand() & 255) << 8 | (rand() & 255));
        m_Player[i].LoadKeys(i + 1);
        m_Player[i].Position(RandFloat(50, 270), RandFloat(50, 150));
        m_Player[i].m_SP = &m_SysPtr;
    }

    m_SysPtr.Particle = m_ParticleSystem;
    m_SysPtr.Player = m_Player;
    m_SysPtr.Sound = m_SoundSystem;
    m_SysPtr.Weapon = m_WeaponSystem;
    m_SysPtr.Options = &m_GlobalOptions;

    m_ParticleSystem->m_SP = &m_SysPtr;
    m_WeaponSystem->m_SP = &m_SysPtr;

    return (DE_OK);
}

void CpumApp::ResetGame(int l)
{
    LoadOptions(&m_GlobalOptions);
    if (m_Level)
    {
        delete m_Level;
    }

    m_SysPtr.Particle->Reset();
    m_SysPtr.Weapon->Reset();

    for (int i = 0; i < m_GlobalOptions.Players; i++)
    {
        m_Player[i].SetColor((rand() & 255) << 16 | (rand() & 255) << 8 | (rand() & 255));
        m_Player[i].LoadKeys(i + 1);
        m_Player[i].Position(RandFloat(50, 270), RandFloat(50, 150));
        m_Player[i].m_SP = &m_SysPtr;
    }

    m_Level = new CPumLevel;
    m_Level->Load(m_LevelList[l]);

    m_SysPtr.Level = m_Level;
}

// OnExit()
// Called by DieselEngine, just before it cleans up
// this is good place to clean up your resources
void CpumApp::OnExit()
{
    int i;

    if (m_Player)
    {
        delete[] m_Player;
    }
    if (m_ParticleSystem)
    {
        delete m_ParticleSystem;
    }
    if (m_SoundSystem)
    {
        delete m_SoundSystem;
    }
    if (m_WeaponSystem)
    {
        delete m_WeaponSystem;
    }
    if (m_Level)
    {
        delete m_Level;
    }
    for (i = 0; i < m_Levels; i++)
    {
        if (m_LevelList[i])
        {
            delete[] m_LevelList[i];
        }
    }
    if (m_LevelList)
    {
        delete[] m_LevelList;
    }

    m_Menuback.Release();
    m_Backbuffer.Release();
    m_Backbuffer2.Release();
    m_Mouse.Shutdown();
    m_Keyboard.Shutdown();
    m_Input.Shutdown();
}


//////////////////////////////////////////////////////////////////////
// Updating
//////////////////////////////////////////////////////////////////////

// OnFlip()
// Application main loop
void CpumApp::OnFlip()
{
    enum GAMESTATE { eGame, eMainmenu, eOptions, eGameexit, eSetupkeys };
    static int GameState = eMainmenu,
                    tmp;
    static float muzik = 10.0f,
                    ang;

    ang += m_fFrameTime * TimeFactor;

    muzik += m_fFrameTime;
    if (muzik > 1.0f)
    {
        m_SysPtr.Sound->UpdateMusic();
        muzik = 0.0f;
    }

    m_SysPtr.Sound->SlowTime(m_GlobalOptions.SoundFrequency);

    switch (GameState)
    {
        case eGame:
            if (!GameLoop())
            {
                GameState = eGameexit;
            }
            break;

        case eMainmenu:
            tmp = MainMenu(&ang);
            if (tmp == 1)
            {
                ResetGame(lv);
                GameState = eGame;
            }
            else if (tmp == 2)
            {
                GameState = eOptions;
            }
            else if (tmp == 3)
            {
                Shutdown();
            }
            break;

        case eOptions:
            tmp = OptionsMenu(&ang);
            if (tmp == 1)
            {
                GameState = eMainmenu;
            }
            if (tmp == 2)
            {
                GameState = eSetupkeys;
                KeySetup(&ang, 1);
            }
            break;

        case eSetupkeys:
            tmp = KeySetup(&ang, 2);
            if (tmp == 1)
            {
                GameState = eOptions;
            }
            break;

        case eGameexit:
            tmp = GameExit();

            if (tmp == 1)
            {
                GameState = eMainmenu;
            }
            if (tmp == 2)
            {
                GameState = eGame;
            }
    }
}

int CpumApp::GameLoop()
{
    int i;

    DWORD* ptr;

    if (m_Keyboard.Update(m_dwTimeGetTimeVal) == DE_OK)
    {
        for (i = 0; i < m_SysPtr.Options->Players; i++)
        {
            m_Player[i].ReadKeys(&m_Keyboard);
        }

        if (m_Keyboard.GetButton(DIK_ESCAPE, NULL) == TRUE)
        {
            // Device button 'Start' is down...shutdown the application
            return (0);
        }
    }

    if (globalMatrixTime < 1.0f)
    {
        globalMatrixTime *= 1.015f;
    }

    if (globalMatrixTime > 1.0f)
    {
        globalMatrixTime = 1.0f;
    }


    m_Backbuffer.BltFast(0, 0, &m_Level->m_srfBackground, NULL, 0);
    m_Backbuffer.Blt(NULL, &m_Level->m_srfGfx, NULL, DE_BLTSRCCOLORKEY);

    ptr = (DWORD*)m_Level->m_srfGfx.Lock(NULL);

    m_Level->Regenerate(ptr, m_SysPtr.Options->LevelRegenSpeed, m_SysPtr.Options->BloodVanishSpeed);

    for (i = 0; i < m_SysPtr.Options->Players; i++)
    {
        m_Player[i].UpdateControls(ptr);
        m_Player[i].Update(ptr, m_fFrameTime * globalMatrixTime);
    }

    m_ParticleSystem->Update(m_fFrameTime * globalMatrixTime);
    m_WeaponSystem->UpdateB(m_fFrameTime * globalMatrixTime);

    m_Level->m_srfGfx.Unlock();

    ptr = (DWORD*)m_Backbuffer.Lock(NULL);

    if (m_SysPtr.Options->BlendParticles == true)
    {
        m_ParticleSystem->DrawBlended(ptr);
    }
    else
    {
        m_ParticleSystem->Draw(ptr);
    }

    m_WeaponSystem->DrawB(ptr);

    for (i = 0; i < m_SysPtr.Options->Players; i++)
    {
        m_Player[i].Draw(ptr);
    }

    /*
    #ifdef _DEBUG
        char buffer[32];

        sprintf(buffer, "%.0f ", GetFps());
        LilString( 0, 0, buffer, 0x00ffffff, ptr );
        _stprintf(buffer, "%.2f ", m_fFrameTime);
        LilString( 0, 6, buffer, 0x00ffffff, ptr );
    #endif
    */

    m_Backbuffer.Unlock();


    if (globalMatrixTime == 1.0f)
    {
        m_Backbuffer2.BltFast(0, 0, &m_Backbuffer, NULL, 0);
    }
    else
    {
        m_Backbuffer2.AlphaBlend(0, 0, &m_Backbuffer, NULL, globalMatrixTime);
    }

    m_srfBack.BltFast(0, 0, &m_Backbuffer2, NULL, 0);

    return (1);
}

int CpumApp::MainMenu(float* a)
{
    static int info = 0;
    static float keydelay = 0.0f,
                    infodelay = 4.0f;
    DWORD* ptr;
    int i, j;
    RECT r;

    char* InfoText[9] =
    {
        "pikku-ukot mesoo v1.0",
        "kopyrikht (c) 10tons entertainment 2001",
        "intter netsku: www.10tons.org",
        "koodin ja gfx:n tuotti milzer",
        "musiqja lahjoitti crud",
        "turbiksen kartat myonsi mikko virta meille",
        "ma tykkaan maileista ja oluesta, milzer(a)10tons.org",
        "terkquja kaikille upeille, en jaksa listata :) (dokuissa on)",
        " "
    };

    keydelay -= m_fFrameTime;
    infodelay -= m_fFrameTime;

    if (infodelay < 0.0f)
    {
        ++info %= 9;
        infodelay = 4.0f;
    }

    if (m_Keyboard.Update(m_dwTimeGetTimeVal) == DE_OK)
    {
        if (m_Keyboard.GetButton(DIK_F1, NULL) == TRUE)
        {
            return (1);
        }
        if (m_Keyboard.GetButton(DIK_F2, NULL) == TRUE)
        {
            return (2);
        }
        if (m_Keyboard.GetButton(DIK_F3, NULL) == TRUE && keydelay < 0.0f)
        {
            keydelay = 0.1f;
            ++lv %= m_Levels;
        }
        if (m_Keyboard.GetButton(DIK_ESCAPE, NULL) == TRUE)
        {
            return (3);
        }
    }

    RoundToInt(&i, *a * 0.7f);
    RoundToInt(&j, *a * 0.8f);

    r.left = (long)(cosinus[i & 255] * 160 + 160);
    r.top = (long)(sinus[j & 255] * 100 + 100);

    r.right = r.left + 320;
    r.bottom = r.top + 200;

    //m_Backbuffer.AlphaBlend( 0, 0, &m_Menuback, &r, sinus[ (int)(*a*0.364f)&255 ] * 0.3f + 0.4f );
    m_Backbuffer.BltFast(0, 0, &m_Menuback, &r, 0);

    r.left = (long)(sinus[(i + 140) & 255] * 160 + 160);
    r.top = (long)(cosinus[(j + 100) & 255] * 100 + 100);

    r.right = r.left + 320;
    r.bottom = r.top + 200;

    m_Backbuffer.AlphaBlend(0, 0, &m_Menuback, &r, cosinus[(int)(*a * 0.364f) & 255] * 0.5f + 0.5f);

    ptr = (DWORD*)m_Backbuffer.Lock(NULL);

    LilStringCenterX(70, " f1 - aloita   ", 0x00ffffff, ptr);
    LilStringCenterX(80, " f2 - asetukset", 0x00ffffff, ptr);
    LilStringCenterX(90, " f3 - kartta   ", 0x00ffffff, ptr);
    LilStringCenterX(100, m_LevelList[lv] + 4, 0x00ffffff, ptr);

    LilStringCenterX(120, "esc - pois     ", 0x00ffffff, ptr);

    LilStringCenterX(180, InfoText[info], 0x00ffffff, ptr);

    m_Backbuffer.Unlock();

    m_srfBack.BltFast(0, 0, &m_Backbuffer, NULL, 0);

    return (0);
}

int CpumApp::OptionsMenu(float* a)
{
    static int col[32],
           selection = 0;
    static float keydelay = 0.1f;
    DWORD* ptr;
    int i, j,
            kei = 0;
    RECT r;
    char buf[64];

    keydelay -= m_fFrameTime;
    kei = 0;

    if (m_Keyboard.Update(m_dwTimeGetTimeVal) == DE_OK && keydelay < 0.0f)
    {
        if (m_Keyboard.GetButton(DIK_UP, NULL) == TRUE)
        {
            selection--;
            keydelay = 0.1f;
        }

        if (m_Keyboard.GetButton(DIK_DOWN, NULL) == TRUE)
        {
            selection++;
            keydelay = 0.1f;
        }

        if (m_Keyboard.GetButton(DIK_LEFT, NULL) == TRUE)
        {
            kei = -1;
            keydelay = 0.1f;
        }

        if (m_Keyboard.GetButton(DIK_RIGHT, NULL) == TRUE)
        {
            kei = 1;
            keydelay = 0.1f;
        }

        if (m_Keyboard.GetButton(DIK_RETURN, NULL) == TRUE)
        {
            kei = 1;
            keydelay = 0.1f;
        }

        if (m_Keyboard.GetButton(DIK_BACK, NULL) == TRUE)
        {
            kei = -1;
            keydelay = 0.1f;
        }

        if (m_Keyboard.GetButton(DIK_ESCAPE, NULL) == TRUE)
        {
            selection = 15;
            keydelay = 0.1f;
        }
    }

    selection = Crop(selection, 0, 16);

    for (i = 0; i < 32; i++)
    {
        if (col[i] >= 0)
        {
            col[i] -= (int)(m_fFrameTime * 300);
        }
        col[i] = Crop(col[i], 0, 255);
    }

    col[selection] = (rand() & 31) + 224;


    RoundToInt(&i, *a * 0.7f);
    RoundToInt(&j, *a * 0.8f);

    r.left = (long)(cosinus[i & 255] * 160 + 160);
    r.top = (long)(sinus[j & 255] * 100 + 100);

    r.right = r.left + 320;
    r.bottom = r.top + 200;

    //m_Backbuffer.AlphaBlend( 0, 0, &m_Menuback, &r, sinus[ (int)(*a*0.364f)&255 ] * 0.3f + 0.4f );
    m_Backbuffer.BltFast(0, 0, &m_Menuback, &r, 0);

    r.left = (long)(sinus[(i + 140) & 255] * 160 + 160);
    r.top = (long)(cosinus[(j + 100) & 255] * 100 + 100);

    r.right = r.left + 320;
    r.bottom = r.top + 200;

    m_Backbuffer.AlphaBlend(0, 0, &m_Menuback, &r, cosinus[(int)(*a * 0.364f) & 255] * 0.5f + 0.5f);


    switch (selection)
    {
        case 0:
            m_SysPtr.Options->Gravity += kei * 5;
            break;

        case 1:
            m_SysPtr.Options->BloodFactor += (float)kei * 0.1f;
            if (m_SysPtr.Options->BloodFactor < 0.0f)
            {
                m_SysPtr.Options->BloodFactor = 0.0f;
            }
            break;

        case 2:
            m_SysPtr.Options->ParticleFactor += (float)kei * 0.1f;
            if (m_SysPtr.Options->ParticleFactor < 0.2f)
            {
                m_SysPtr.Options->ParticleFactor = 0.2f;
            }
            break;

        case 3:
            m_SysPtr.Options->LevelRegenSpeed += kei;
            m_SysPtr.Options->LevelRegenSpeed = Crop(m_SysPtr.Options->LevelRegenSpeed, 0, 256);
            break;

        case 4:
            m_SysPtr.Options->BloodVanishSpeed += kei * 2;
            m_SysPtr.Options->BloodVanishSpeed = Crop(m_SysPtr.Options->BloodVanishSpeed, 0, 512);
            break;

        case 5:
            m_SysPtr.Options->Particles += kei * 100;
            m_SysPtr.Options->Particles = Crop(m_SysPtr.Options->Particles, 500, 20000);
            break;

        case 6:
            m_SysPtr.Options->Bullets += kei * 100;
            m_SysPtr.Options->Bullets = Crop(m_SysPtr.Options->Bullets, 500, 10000);
            break;

        case 7:
            m_SysPtr.Options->Players += kei;
            m_SysPtr.Options->Players = Crop(m_SysPtr.Options->Players, 1, 3);
            break;

        case 8:
            m_SysPtr.Options->MusicVolume += kei * 2;
            m_SysPtr.Options->MusicVolume = Crop(m_SysPtr.Options->MusicVolume, 0, 255);
            m_SysPtr.Sound->UpdateVolume();
            break;

        case 9:
            if (kei == -1)
            {
                if (m_SysPtr.Options->SoundOutput == FSOUND_OUTPUT_DSOUND)
                {
                    m_SysPtr.Options->SoundOutput = FSOUND_OUTPUT_NOSOUND;
                }

                if (m_SysPtr.Options->SoundOutput == FSOUND_OUTPUT_WINMM)
                {
                    m_SysPtr.Options->SoundOutput = FSOUND_OUTPUT_DSOUND;
                }
            }

            if (kei == 1)
            {
                if (m_SysPtr.Options->SoundOutput == FSOUND_OUTPUT_DSOUND)
                {
                    m_SysPtr.Options->SoundOutput = FSOUND_OUTPUT_WINMM;
                }

                if (m_SysPtr.Options->SoundOutput == FSOUND_OUTPUT_NOSOUND)
                {
                    m_SysPtr.Options->SoundOutput = FSOUND_OUTPUT_DSOUND;
                }
            }
            break;

        case 10:
            if (kei == -1)
            {
                m_SysPtr.Options->SoundFrequency /= 2;
            }

            if (kei == 1)
            {
                m_SysPtr.Options->SoundFrequency *= 2;
            }

            m_SysPtr.Options->SoundFrequency = Crop(m_SysPtr.Options->SoundFrequency, 11025, 44100);
            break;

        case 11:
            m_SysPtr.Options->SoundChannels += kei * 2;
            m_SysPtr.Options->SoundChannels = Crop(m_SysPtr.Options->SoundChannels, 0, 128);
            break;

        case 12:
            if (kei == -1)
            {
                m_SysPtr.Options->BlendParticles = false;
            }
            if (kei == 1)
            {
                m_SysPtr.Options->BlendParticles = true;
            }
            break;

        case 13:
            if (kei == 1)
            {
                SetDefaultOptions(m_SysPtr.Options);
            }
            break;


        case 14:
            if (kei == 1)
            {
                return (2);
            }
            break;

        case 15:
            if (kei == 1)
            {
                SaveOptions(m_SysPtr.Options);
                return (1);
            }
            break;

        case 16:
            if (kei == 1)
            {
                LoadOptions(m_SysPtr.Options);
                return (1);
            }
            break;

    }

    ptr = (DWORD*)m_Backbuffer.Lock(NULL);

#define MenuRow(y,str,val,num) LilStringR(150,y,str,(col[num]<<16)|(col[num]<<8)|col[num],ptr);LilString(170,y,val,(col[num]<<16)|(col[num]<<8)|col[num],ptr);LilString(150,y,"....",(col[num]<<16)|(col[num]<<8)|col[num],ptr);

    LilStringCenterX(10, " - peliin liittyva matsku -", 0x00ffffff, ptr);

    sprintf(buf, "%.2f", m_SysPtr.Options->Gravity / 17.227319f);
    MenuRow(20, "painovoima", buf, 0);

    sprintf(buf, "%.1f", m_SysPtr.Options->BloodFactor);
    MenuRow(26, "verikerroin", buf, 1);

    sprintf(buf, "%.1f", m_SysPtr.Options->ParticleFactor);
    MenuRow(32, "partiklekerroin", buf, 2);

    MenuRow(38, "kentan korjaus", itoa(m_SysPtr.Options->LevelRegenSpeed, buf, 10), 3);
    MenuRow(44, "veren siivous", itoa(m_SysPtr.Options->BloodVanishSpeed, buf, 10), 4);
    MenuRow(50, "partikleja", itoa(m_SysPtr.Options->Particles, buf, 10), 5);
    MenuRow(56, "ammuksia", itoa(m_SysPtr.Options->Bullets, buf, 10), 6);
    MenuRow(62, "pelaajia", itoa(m_SysPtr.Options->Players, buf, 10), 7);

    LilStringCenterX(82, " - audio eli aeaeni -", 0x00ffffff, ptr);
    MenuRow(92, "musiikin voimakqus", itoa(m_SysPtr.Options->MusicVolume, buf, 10), 8);

    switch (m_SysPtr.Options->SoundOutput)
    {
        case FSOUND_OUTPUT_NOSOUND:
            strcpy(buf, "ihan hiljasta");
            break;
        case FSOUND_OUTPUT_DSOUND:
            strcpy(buf, "direktsound");
            break;
        case FSOUND_OUTPUT_WINMM:
            strcpy(buf, "vindous multimedia waveout");
            break;
    }

    MenuRow(98, "ulosvehje", buf, 9);
    MenuRow(104, "taajuus", itoa(m_SysPtr.Options->SoundFrequency, buf, 10), 10);
    MenuRow(110, "kanavia", itoa(m_SysPtr.Options->SoundChannels, buf, 10), 11);

    LilStringCenterX(130, " - visuaalista matskua -", 0x00ffffff, ptr);
    MenuRow(140, "hienot efut", m_SysPtr.Options->BlendParticles ? "juu" : "ei", 12);

    LilStringCenterX(164, "normaalit asetukset", (col[13] << 16) | (col[13] << 8) | col[13], ptr);
    LilStringCenterX(174, "muuta nappulat", (col[14] << 16) | (col[14] << 8) | col[14], ptr);
    LilStringCenterX(184, "tallenna ja poistu", (col[15] << 16) | (col[15] << 8) | col[15], ptr);
    LilStringCenterX(190, "tunge rektumiin", (col[16] << 16) | (col[16] << 8) | col[16], ptr);

    m_Backbuffer.Unlock();

    m_srfBack.BltFast(0, 0, &m_Backbuffer, NULL, 0);

    return (0);
}

int CpumApp::KeySetup(float* a, int rektum)
{
    DWORD* ptr;
    int i, j, kei, retval = 3;
    static int state;
    static float keydelay = 0.2f;
    static BYTE keys[8];
    static char txt[32];
    RECT r;
    FILE* fp;

    if (rektum == 1)
    {
        state = 0;
        strcpy(txt, "pelaaja 1 vasen");
    }

    keydelay -= m_fFrameTime;
    kei = -1;

    if (m_Keyboard.Update(m_dwTimeGetTimeVal) == DE_OK && keydelay < 0.0f)
    {
        for (i = 0; i < 256; i++)
        {
            if (m_Keyboard.GetButton(i, NULL) == TRUE)
            {
                kei = i;
                i = 256;
                keydelay = 0.2f;
            }
        }
    }

    RoundToInt(&i, *a * 0.7f);
    RoundToInt(&j, *a * 0.8f);

    r.left = (long)(cosinus[i & 255] * 160 + 160);
    r.top = (long)(sinus[j & 255] * 100 + 100);

    r.right = r.left + 320;
    r.bottom = r.top + 200;

    m_Backbuffer.AlphaBlend(0, 0, &m_Menuback, &r, sinus[(int)(*a * 0.364f) & 255] * 0.3f + 0.4f);

    r.left = (long)(sinus[(i + 140) & 255] * 160 + 160);
    r.top = (long)(cosinus[(j + 100) & 255] * 100 + 100);

    r.right = r.left + 320;
    r.bottom = r.top + 200;

    m_Backbuffer.AlphaBlend(0, 0, &m_Menuback, &r, cosinus[(int)(*a * 0.364f) & 255] * 0.3f + 0.4f);

    if (kei > -1)
    {
        switch (state)
        {
            case 0:
                strcpy(txt, "pelaaja 1 oikea");
                keys[PLR_C_LEFT] = kei;
                break;
            case 1:
                strcpy(txt, "pelaaja 1 ylos");
                keys[PLR_C_RIGHT] = kei;
                break;
            case 2:
                strcpy(txt, "pelaaja 1 alas");
                keys[PLR_C_UP] = kei;
                break;
            case 3:
                strcpy(txt, "pelaaja 1 aseen vaihto");
                keys[PLR_C_DOWN] = kei;
                break;
            case 4:
                strcpy(txt, "pelaaja 1 hyppy");
                keys[PLR_C_CHANGE] = kei;
                break;
            case 5:
                strcpy(txt, "pelaaja 1 ampuminen");
                keys[PLR_C_JUMP] = kei;
                break;
            case 6:
                strcpy(txt, "pelaaja 2 vasen");
                keys[PLR_C_SHOOT] = kei;
                fp = fopen("keys.1", "wb");
                if (fp)
                {
                    fwrite(keys, sizeof(keys), 1, fp);
                    fflush(fp);
                    fclose(fp);
                }
                break;


            case 7:
                strcpy(txt, "pelaaja 2 oikea");
                keys[PLR_C_LEFT] = kei;
                break;
            case 8:
                strcpy(txt, "pelaaja 2 ylos");
                keys[PLR_C_RIGHT] = kei;
                break;
            case 9:
                strcpy(txt, "pelaaja 2 alas");
                keys[PLR_C_UP] = kei;
                break;
            case 10:
                strcpy(txt, "pelaaja 2 aseen vaihto");
                keys[PLR_C_DOWN] = kei;
                break;
            case 11:
                strcpy(txt, "pelaaja 2 hyppy");
                keys[PLR_C_CHANGE] = kei;
                break;
            case 12:
                strcpy(txt, "pelaaja 2 ampuminen");
                keys[PLR_C_JUMP] = kei;
                break;
            case 13:
                strcpy(txt, "pelaaja 3 vasen");
                keys[PLR_C_SHOOT] = kei;
                fp = fopen("keys.2", "wb");
                if (fp)
                {
                    fwrite(keys, sizeof(keys), 1, fp);
                    fflush(fp);
                    fclose(fp);
                }
                break;


            case 14:
                strcpy(txt, "pelaaja 3 oikea");
                keys[PLR_C_LEFT] = kei;
                break;
            case 15:
                strcpy(txt, "pelaaja 3 ylos");
                keys[PLR_C_RIGHT] = kei;
                break;
            case 16:
                strcpy(txt, "pelaaja 3 alas");
                keys[PLR_C_UP] = kei;
                break;
            case 17:
                strcpy(txt, "pelaaja 3 aseen vaihto");
                keys[PLR_C_DOWN] = kei;
                break;
            case 18:
                strcpy(txt, "pelaaja 3 hyppy");
                keys[PLR_C_CHANGE] = kei;
                break;
            case 19:
                strcpy(txt, "pelaaja 3 ampuminen");
                keys[PLR_C_JUMP] = kei;
                break;
            case 20:
                keys[PLR_C_SHOOT] = kei;
                fp = fopen("keys.3", "wb");
                if (fp)
                {
                    fwrite(keys, sizeof(keys), 1, fp);
                    fflush(fp);
                    fclose(fp);
                }
                return (1);
                break;
        }

        state++;
    }


    ptr = (DWORD*)m_Backbuffer.Lock(NULL);

    LilStringCenterX(80, "painahan toki haluamas nappula?", 0x00ffffff, ptr);
    LilStringCenterX(90, txt, 0x00ffffff, ptr);

    m_Backbuffer.Unlock();

    m_srfBack.BltFast(0, 0, &m_Backbuffer, NULL, 0);

    return (0);
}

int CpumApp::GameExit()
{
    DWORD* ptr;

    if (m_Keyboard.Update(m_dwTimeGetTimeVal) == DE_OK)
    {
        if (m_Keyboard.GetButton(DIK_F10, NULL) == TRUE)
        {
            return (1);
        }
        if (m_Keyboard.GetButton(DIK_F9, NULL) == TRUE)
        {
            return (2);
        }
    }

    ptr = (DWORD*)m_Backbuffer.Lock(NULL);

    Blur(ptr, 600);

    LilStringCenterXo(80, " f9 - jatka ", 0x00ffffff, ptr);
    LilStringCenterXo(90, "f10 - lopeta", 0x00ffffff, ptr);

    m_Backbuffer.Unlock();

    m_srfBack.BltFast(0, 0, &m_Backbuffer, NULL, 0);

    return (0);
}

void CpumApp::Blur(DWORD* ptr, int c)
{
    int p, i, r, g, b, r2, g2, b2;

    for (i = 0; i < c; i++)
    {
        p = RandInt(1, 198) * ScreenWidth + RandInt(1, 318);

        switch (rand() & 7)
        {
            case 0:
                r = g = b = ptr[p + 320];
                break;
            case 1:
                r = g = b = ptr[p - 320];
                break;
            case 2:
                r = g = b = ptr[p - 1];
                break;
            case 3:
                r = g = b = ptr[p + 1];
                break;
            case 4:
                r = g = b = ptr[p + 321];
                break;
            case 5:
                r = g = b = ptr[p - 321];
                break;
            case 6:
                r = g = b = ptr[p - 319];
                break;
            case 7:
                r = g = b = ptr[p + 319];
                break;
        }

        r2 = g2 = b2 = ptr[p];

        r = (r & 0x00ff0000) >> 16;
        g = (g & 0x0000ff00) >> 8;
        b = b & 0x000000ff;

        r2 = (r2 & 0x00ff0000) >> 16;
        g2 = (g2 & 0x0000ff00) >> 8;
        b2 = b2 & 0x000000ff;

        r = (r + r2) >> 1;
        g = (g + g2) >> 1;
        b = (b + b2) >> 1;

        if ((r2 - r) + (g2 - g) + (b2 - b) < 0)
        {
            ptr[p] = (r << 16) | (g << 8) | b;
        }
    }

    for (p = 0; p < 320; p++)
    {
        ptr[p] = 0;
    }
    for (p = 63680; p < 64000; p++)
    {
        ptr[p] = 0;
    }
    for (p = 0; p < 64000; p += 320)
    {
        ptr[p] = 0;
    }
    for (p = 319; p < 64000; p += 320)
    {
        ptr[p] = 0;
    }
}

DE_RETVAL CpumApp::GetLevels()
{
    struct _finddata_t fblock;
    long h = 0,
            i = 0;

    h = _findfirst("plv\\*.plv", &fblock);

    if (h == -1)
    {
        return DE_FILENOTFOUND;
    }

    m_Levels = 1;

    while (_findnext(h, &fblock) == 0)
    {
        m_Levels++;
    }

    _findclose(h);

    m_LevelList = new char* [m_Levels];

    h = _findfirst("plv\\*.plv", &fblock);

    m_LevelList[i] = new char[strlen(fblock.name) + 5];
    strcpy(m_LevelList[i], "plv\\");
    strcat(m_LevelList[i], fblock.name);
    i++;

    while (_findnext(h, &fblock) == 0)
    {
        m_LevelList[i] = new char[strlen(fblock.name) + 5];
        strcpy(m_LevelList[i], "plv\\");
        strcat(m_LevelList[i], fblock.name);
        i++;
    }

    _findclose(h);

    return (DE_OK);
}

void CpumApp::CompileLevels()
{
    CPumLevel* l;
    FILE* fp;
    char plv[256],
                bg[256],
                gfx[256],
                tmp[256];

    fp = fopen("uudet\\lista.txt", "rt");
    if (!fp)
    {
        return;
    }

    while (!feof(fp))
    {
        strcpy(bg, "uudet\\");
        strcpy(gfx, "uudet\\");
        strcpy(plv, "plv\\");

        if (fscanf(fp, "%s", tmp) == EOF)
        {
            break;
        }
        strcat(plv, tmp);

        if (fscanf(fp, "%s", tmp) == EOF)
        {
            break;
        }
        strcat(gfx, tmp);

        if (fscanf(fp, "%s", tmp) == EOF)
        {
            break;
        }
        strcat(bg, tmp);

        l = new CPumLevel;
        l->Load(gfx, bg);
        l->Save(plv);
        delete l;
    }

    fclose(fp);
}


//////////////////////////////////////////////////////////////////////
// Overrided Message handlers
//////////////////////////////////////////////////////////////////////

