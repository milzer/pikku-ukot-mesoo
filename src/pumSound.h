#ifndef __PUMSOUND_H__
#define __PUMSOUND_H__

#define MAX_SAMPLES 128

#include <stdio.h>
#include "fmod.h"
#include "fmod_errors.h"
#include "pumGlobals.h"

class CPumSoundSystem
{
public:
    FMUSIC_MODULE* m_Music;
    FSOUND_SAMPLE* m_Sample[MAX_SAMPLES];
    int m_MusicVol;
    int* m_PlayList;
    int m_SongCount;
    char** m_SongList;

    void UpdateMusic();
    void LoadMusic(const char* f);
    void PlayMusic();
    void PlayMusic(const char* f);
    void StopMusic();
    void LoadSample(int id, const char* f);
    void PlaySample(int id, int x);
    void PlaySample(int id1, int id2, int x);
    void LoadSoundScript(const char* f);
    void GetMusics();
    void UpdateVolume();
    void SlowTime(int defaultfreq);

    CPumSoundSystem(SDE_PUMOPTIONS* opt);
    ~CPumSoundSystem();
};

#endif