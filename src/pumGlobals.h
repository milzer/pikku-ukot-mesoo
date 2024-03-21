#ifndef __PUMGLOBALS_H__
#define __PUMGLOBALS_H__

#include <DieselVector2.h>
#include <DieselSurface.h>
#include <stdlib.h>

class   CPumParticleSystem;
class   CPumSoundSystem;
class   CPumPlayer;
class   CPumLevel;
class   CPumWeaponSystem;

struct SDE_PUMOPTIONS {
    float   Gravity,
    BloodFactor,
    ParticleFactor;

    int     LevelRegenSpeed,
    BloodVanishSpeed,
    Particles,
    Bullets,
    Players,
    MusicVolume,
    SoundOutput,
    SoundFrequency,
    SoundChannels;

    bool    BlendParticles;
    float   MatrixTime;
};

class   CPumSysPointers
{
public:
    CPumParticleSystem  *Particle;
    CPumSoundSystem     *Sound;
    CPumPlayer          *Player;
    CPumLevel           *Level;
    CPumWeaponSystem    *Weapon;
    SDE_PUMOPTIONS      *Options;

    CPumSysPointers() {};
    ~CPumSysPointers() {};
};

#define TimeFactor      65.0f
#define ScreenWidth     320
#define ScreenHeight    200

#define Swap(x,y)   x^=y^=x^=y
#define Crop(a,b,c) (((a)<(b))?(b):((a)>(c))?(c):(a))
#define PiD128      0.024543692606f
#define RoundDef(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))
#define Max(a,b)    ((a)>(b)?(a):(b))
#define Min(a,b)    ((a)<(b)?(a):(b))

extern float    cosinus[256],
sinus[256];

void            SetDefaultOptions(SDE_PUMOPTIONS *o);
void            LoadOptions(SDE_PUMOPTIONS *o);
void            SaveOptions(SDE_PUMOPTIONS *o);
float           Distance(float x1, float y1, float x2, float y2);
void            InitSinCos();
void            WallBounce(CDieselVector2 *spd, int x, int y, CHAR *mask, float frameTime);
void            RoundToInt(int *int_pointer, float f);

__forceinline float Angle(float x1, float y1, float x2, float y2)
{
    float   i, j, l;

    i = (y1 - y2);
    j = (x1 - x2);
    l = (float) atan2(i, j);
    return((float) 40.7436654315f *(l + 3.14159265359f));
}


__forceinline void RoundToInt(int *int_pointer, float f)
{
    __asm  fld  f
    __asm  mov  edx, int_pointer
    __asm  FRNDINT
    __asm  fistp dword ptr [edx];
}

#endif