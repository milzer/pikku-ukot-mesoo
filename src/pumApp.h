#ifndef __PUMAPP_H__
#define __PUMAPP_H__


#include <time.h>
#include <Diesel.h>
#include <DieselGraphics.h>
#include <DieselInput.h>

#include "pumGlobals.h"
#include "pumSound.h"
#include "pumParticle.h"
#include "pumLevel.h"
//#include "pumLilfont.h"
#include "pumPlayer.h"

class CpumApp : public IDieselApplication
{
public:
    void Blur(DWORD* ptr, int c);
    int GameExit();
    int GameLoop();
    int MainMenu(float* a);
    int OptionsMenu(float* a);
    int InitGame(SDE_PUMOPTIONS* opt);
    int KeySetup(float* a, int rektum);
    void ResetGame(int l);
    DE_RETVAL GetLevels();
    void CompileLevels();

    CpumApp();
    virtual ~CpumApp();

    void OnFlip();

    CDieselInput m_Input;
    CDieselInputKeyboard m_Keyboard;
    CDieselInputMouse m_Mouse;

    DE_RETVAL OnInitDone();
    void OnExit();

protected:

    CPumLevel* m_Level;
    CPumPlayer* m_Player;
    CDieselSurface m_Backbuffer;
    CDieselSurface m_Backbuffer2;
    CDieselSurface m_Menuback;
    CPumParticleSystem* m_ParticleSystem;
    CPumSoundSystem* m_SoundSystem;
    CPumWeaponSystem* m_WeaponSystem;
    SDE_PUMOPTIONS m_GlobalOptions;
    CPumSysPointers m_SysPtr;


    float m_fMatrixTime;
    char** m_LevelList;
    int m_Levels;
    int lv;

};

#endif
