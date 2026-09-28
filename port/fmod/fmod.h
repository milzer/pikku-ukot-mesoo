// The subset of the FMOD 3 API that pikku-ukot mesoo uses, reimplemented on
// SDL3_mixer. Tracker modules (the game's .mus files) are played through
// SDL3_mixer's libxmp decoder.

#ifndef __FMOD_H__
#define __FMOD_H__

typedef struct FSOUND_SAMPLE FSOUND_SAMPLE;
typedef struct FMUSIC_MODULE FMUSIC_MODULE;

enum FSOUND_OUTPUTTYPES
{
    FSOUND_OUTPUT_NOSOUND,
    FSOUND_OUTPUT_WINMM,
    FSOUND_OUTPUT_DSOUND,
    FSOUND_OUTPUT_A3D,
};

enum FSOUND_MIXERTYPES
{
    FSOUND_MIXER_AUTODETECT,
    FSOUND_MIXER_BLENDMODE,
    FSOUND_MIXER_MMXP5,
    FSOUND_MIXER_MMXP6,
    FSOUND_MIXER_QUALITY_AUTODETECT,
    FSOUND_MIXER_QUALITY_FPU,
    FSOUND_MIXER_QUALITY_MMXP5,
    FSOUND_MIXER_QUALITY_MMXP6,
};

// Channel indices
#define FSOUND_FREE         -1
#define FSOUND_UNMANAGED    -2
#define FSOUND_ALL          -3

// Sample modes
#define FSOUND_2D           0x00002000

signed char FSOUND_SetOutput(int outputtype);
signed char FSOUND_SetDriver(int driver);
signed char FSOUND_SetMixer(int mixer);
signed char FSOUND_Init(int mixrate, int maxsoftwarechannels, unsigned int flags);
void FSOUND_Close();
int FSOUND_GetMaxChannels();

FSOUND_SAMPLE* FSOUND_Sample_Load(int index, const char* name, unsigned int mode, int memlength);
void FSOUND_Sample_Free(FSOUND_SAMPLE* sptr);
signed char FSOUND_Sample_GetDefaults(FSOUND_SAMPLE* sptr, int* deffreq, int* defvol, int* defpan, int* defpri);

// freq, vol (0-255) and pan (0 = left, 255 = right) take -1 for the sample defaults
int FSOUND_PlaySound3DAttrib(int channel, FSOUND_SAMPLE* sptr, int freq, int vol, int pan,
                             float* pos, float* vel);
signed char FSOUND_SetFrequency(int channel, int freq);
// The sample playing on a channel, or NULL when it is idle
FSOUND_SAMPLE* FSOUND_GetCurrentSample(int channel);

FMUSIC_MODULE* FMUSIC_LoadSong(const char* name);
signed char FMUSIC_FreeSong(FMUSIC_MODULE* mod);
signed char FMUSIC_PlaySong(FMUSIC_MODULE* mod);
signed char FMUSIC_StopSong(FMUSIC_MODULE* mod);
signed char FMUSIC_IsFinished(FMUSIC_MODULE* mod);
// Volume 0-256
signed char FMUSIC_SetMasterVolume(FMUSIC_MODULE* mod, int volume);

#endif
