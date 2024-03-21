#include <math.h>
#include "pumGlobals.h"
#include "fmod.h"

float cosinus[256];
float sinus[256];

void SetDefaultOptions(SDE_PUMOPTIONS* o)
{
    o->Gravity = 0.04f * TimeFactor * TimeFactor;
    o->BloodFactor = 1.0f;
    o->ParticleFactor = 1.0f;

    o->LevelRegenSpeed = 4;
    o->BloodVanishSpeed = 32;
    o->Particles = 9000;
    o->Bullets = 3000;
    o->Players = 2;
    o->MusicVolume = 128;
    o->SoundOutput = FSOUND_OUTPUT_DSOUND;
    o->SoundFrequency = 44100;
    o->SoundChannels = 32;

    o->BlendParticles = true;
}

void LoadOptions(SDE_PUMOPTIONS* o)
{
    FILE* fp = fopen("options.kei", "rb");
    if (!fp)
    {
        SetDefaultOptions(o);
        return;
    }

    if (fread(o, 1, sizeof(SDE_PUMOPTIONS), fp) != sizeof(SDE_PUMOPTIONS))
    {
        SetDefaultOptions(o);
    }

    fclose(fp);
}

void SaveOptions(SDE_PUMOPTIONS* o)
{
    FILE* fp = fopen("options.kei", "wb");
    if (!fp)
    {
        return;
    }

    fwrite(o, 1, sizeof(SDE_PUMOPTIONS), fp);

    fflush(fp);
    fclose(fp);
}

float Distance(float x1, float y1, float x2, float y2)
{
    float ax, ay;

    ax = x2 - x1;
    ay = y2 - y1;

    return ((float)sqrt(ax * ax + ay * ay));
}

void WallBounce(CDieselVector2* spd, int x, int y, CHAR* mask, float frameTime)
{
    CDieselVector2 wall(0.0f, 0.0f);
    CDieselVector2 newspd;

    float tmp;
    float r = spd->Length();
    float n;
    int xx;
    int yy;
    int d;

    for (d = 0; d < 256; d += 16)
    {
        RoundToInt(&xx, cosinus[d] * 2 * r * frameTime + x);
        RoundToInt(&yy, sinus[d] * 2 * r * frameTime + y);

        xx += (rand() % 5) - 2;
        yy += (rand() % 5) - 2;

        if (xx >= 0 && xx < ScreenWidth && yy >= 0 && yy < ScreenHeight)
        {
            if (mask[yy * ScreenWidth + xx] != 0)
            {
                wall.Add(CDieselVector2(cosinus[d]*r, sinus[d]*r));
            }
        }
    }

    n = (wall.x * wall.x + wall.y * wall.y);
    if (n == 0)
    {
        return;
    }

    tmp = (spd->x * wall.x + spd->y * wall.y) / n * 2;

    newspd.Set(tmp * wall.x - spd->x, tmp * wall.y - spd->y);

    spd->Set(-newspd.x, -newspd.y);
}

void InitSinCos()
{
    for (int i = 0; i < 256; i++)
    {
        cosinus[i] = (float)cos((float)i * PiD128);
        sinus[i] = (float)sin((float)i * PiD128);
    }
}