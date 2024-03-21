#ifndef __PUMWEAPON_H__
#define __PUMWEAPON_H__

#include <string.h>
#include <DieselVector2.h>
#include "pumGlobals.h"
#include "pumLevel.h"
#include "pumSound.h"


/*
 - nakymaton
 - kierra-pelaaja
 - vahinko (integer) : 1
 - lahtonopeus (float) (float) : 1.0 0.0
 - tulinopeus (float) : 5
 - lipas (integer) : 30
 - latausaika (float) : 60
 - kumi (float) : 0
 - kumiluku (integer) : 0
 - rajahdys (float) : 0
 - rajahdys-ajastus (float) : 0
 - efekti (constant) : null
 - painokerroin (float) : 1.0
 - sirpaleita (integer) (efu) : 0 ""
 - haku (float) : 0
 - haku-ajastus (float) : 0
 - kitka (float) (float) : 1.0 1.0
 - vari (byte) (byte) (byte) : 255 255 255
 - tarkkuus (float) : 0
 - lukumaara (integer) : 1
 - aani-laukaus-id (integer) : 18
 - aani-aktiv-id (integer) : -1
 - aani-seina-id (integer) : -1
*/


#define WPN_SEEKER_ACTIVE   0x0001
#define WPN_BOUNCE_ACTIVE   0x0002
#define WPN_IGNORE_PLR      0x0004
#define WPN_INVISIBLE       0x0008
#define WPN_EFFECT          0x0010

#define WPN_EFF_SPARKBACK   0x0100
#define WPN_EFF_SPARKTRAIL  0x0200
#define WPN_EFF_SPARKS      0x0400
#define WPN_EFF_BLOOD       0x0800
#define WPN_EFF_WALLSPARK   0x1000
#define WPN_EFF_FIRE        0x2000

class CPumWeapon
{
public:
    CPumSysPointers* m_SP;
    CPumWeapon* m_Frag;

    char* m_Name;
    DWORD m_Col;
    unsigned int m_Flags;

    float m_Velocity;
    float m_VelSpread;
    float m_Bounce;
    float m_Blow;
    float m_WeightModifier;
    float m_Seeker;
    float m_FrictionX;
    float m_FrictionY;
    float m_Accuracy;
    float m_Firerate;
    float m_ReloadTime;
    float m_BlowTimer;
    float m_SeekTimer;

    int m_Count;
    int m_Damage;
    int m_Clip;
    int m_BounceCount;
    int m_SoundFireId[2];
    int m_SoundActivId[2];
    int m_SoundWallId[2];
    int m_Fragments;

    void SetDefault();
    void SetDefault(const char* n);

    CPumWeapon();
    CPumWeapon(const char* n);
    ~CPumWeapon();
};

class CPumBullet : public CPumWeapon
{
public:
    CPumSysPointers* m_SP;
    CDieselVector2 m_Vec;

    float m_fXpos;
    float m_fYpos;

    int m_TTL;
    int m_iXpos;
    int m_iYpos;

    char m_Onscreen;

    void Activate(float frameTime);
    void GetWeaponInfo(CPumWeapon* w);
    int Update(float frameTime);
    void Kill();
    void Draw(DWORD* pSurface);
    void Create(float x, float y, float angle, CPumWeapon* w, CPumSysPointers* sp);

    CPumBullet() {};
    ~CPumBullet() {};
};

class CPumWeaponSystem
{
protected:
    int m_MaxB;
    int m_CurrentB;

    CPumBullet* m_Bullets;

public:
    int m_MaxW;

    CPumSysPointers* m_SP;
    CPumWeapon* m_Weapons;

    CPumWeapon* FindWeapon(const char* n);
    void LoadWeapons();
    void ShootB(float x, float y, float a, CPumPlayer* p, CPumWeapon* w);
    void UpdateB(float frameTime);
    void DrawB(DWORD* pSurface);
    void Reset();

    CPumWeaponSystem(const int i);
    ~CPumWeaponSystem();
};

#endif