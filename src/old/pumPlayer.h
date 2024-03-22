#ifndef __PUMPLAYER_H__
#define __PUMPLAYER_H__

#include <DieselInputKeyboard.h>
#include "pumGlobals.h"
#include "pumParticle.h"
#include "pumWeapon.h"
#include "pumLilfont.h"


/*
    Controls:

        walk left,
        walk right,
        aim up,
        aim down,
        change,
        jump,
        shoot
*/

// commands

#define PLR_FRAMES 4

#define PLR_C_LEFT      0
#define PLR_C_RIGHT     1
#define PLR_C_UP        2
#define PLR_C_DOWN      3
#define PLR_C_CHANGE    4
#define PLR_C_JUMP      5
#define PLR_C_SHOOT     6
#define PLR_C_GRAPPLE   7

class CPumPlayer
{
public:
    CPumSysPointers* m_SP;

    DWORD* m_Gfx[4];
    BYTE m_Keys[8];
    BOOL m_KeyBuffer[8];
    char m_Frame;

    float m_WalkSpeed;
    float m_JumpPower;
    float m_AccuracyMod;

    CDieselVector2 m_Vec;

    float m_fXpos;
    float m_fYpos;
    float m_FireDelay;
    float m_WeaponShow;

    int m_iXpos;
    int m_iYpos;
    int m_iAngle;
    int m_Health;
    int m_CurrentWpn;
    BOOL m_Alive;

    void SetKeys(BYTE _left, BYTE _right, BYTE _up, BYTE _down, BYTE _change, BYTE _jump, BYTE _shoot);
    void UpdateControls(DWORD* pSrf);
    void ReadKeys(CDieselInputKeyboard* pKbd);
    int CheckBHit(float fofsx, float fofsy, DWORD* pSrf);
    int HitB(CPumBullet* d);
    int HitP(CPumParticle* d);
    void Update(DWORD* pSrf, float frameTime);
    void Clear();
    void Reset();
    void Position(float x, float y);
    void Draw(DWORD* pSrf);
    inline void EmergencyExit(DWORD* pSrf);
    void SetColor(DWORD c);
    void Damage(int d, CDieselVector2* v);
    void LoadKeys(int i);

    CPumPlayer();
    ~CPumPlayer();
};

#endif