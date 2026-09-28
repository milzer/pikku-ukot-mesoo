#include "fmod.h"
#include "pathcompat.h"

#include <algorithm>
#include <math.h>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

struct FSOUND_SAMPLE
{
    MIX_Audio* audio;
    int freq;
};

struct FMUSIC_MODULE
{
    MIX_Audio* audio;
    MIX_Track* track;
};

struct Channel
{
    MIX_Track* track;
    FSOUND_SAMPLE* sample;
};

// Mix levels. The sound effects are normalised to full scale and dozens can
// overlap in a fight, while the modules are mastered quietly (mix volume
// 48/128), so effects are attenuated and music is lifted to sit under them.
static const float s_SampleGain = 0.45f;
static const float s_MusicGain = 2.5f;

// Output above this level is compressed smoothly towards full scale instead of clipping
static const float s_LimiterKnee = 0.8f;

static int s_Output = FSOUND_OUTPUT_DSOUND;
static MIX_Mixer* s_Mixer = NULL;
static std::vector<Channel> s_Channels;

static void SDLCALL SoftLimit(void* userdata, MIX_Mixer* mixer, const SDL_AudioSpec* spec, float* pcm, int samples)
{
    const float range = 1.0f - s_LimiterKnee;

    for (int i = 0; i < samples; i++)
    {
        float a = fabsf(pcm[i]);
        if (a > s_LimiterKnee)
        {
            pcm[i] = copysignf(s_LimiterKnee + range * tanhf((a - s_LimiterKnee) / range), pcm[i]);
        }
    }
}

signed char FSOUND_SetOutput(int outputtype)
{
    s_Output = outputtype;
    return 1;
}

signed char FSOUND_SetDriver(int driver)
{
    return 1;
}

signed char FSOUND_SetMixer(int mixer)
{
    return 1;
}

signed char FSOUND_Init(int mixrate, int maxsoftwarechannels, unsigned int flags)
{
    if (s_Output == FSOUND_OUTPUT_NOSOUND)
    {
        return 1;
    }

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO) || !MIX_Init())
    {
        return 0;
    }

    SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, mixrate};
    s_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (!s_Mixer)
    {
        return 0;
    }
    MIX_SetPostMixCallback(s_Mixer, SoftLimit, NULL);

    for (int i = 0; i < maxsoftwarechannels; i++)
    {
        s_Channels.push_back({MIX_CreateTrack(s_Mixer), NULL});
    }
    return 1;
}

void FSOUND_Close()
{
    if (!s_Mixer)
    {
        return;
    }

    for (Channel& c : s_Channels)
    {
        MIX_DestroyTrack(c.track);
    }
    s_Channels.clear();

    MIX_DestroyMixer(s_Mixer);
    s_Mixer = NULL;
    MIX_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int FSOUND_GetMaxChannels()
{
    return (int)s_Channels.size();
}

FSOUND_SAMPLE* FSOUND_Sample_Load(int index, const char* name, unsigned int mode, int memlength)
{
    if (!s_Mixer)
    {
        return NULL;
    }

    MIX_Audio* audio = MIX_LoadAudio(s_Mixer, ResolvePath(name).c_str(), true);
    if (!audio)
    {
        return NULL;
    }

    SDL_AudioSpec spec;
    MIX_GetAudioFormat(audio, &spec);
    return new FSOUND_SAMPLE{audio, spec.freq};
}

void FSOUND_Sample_Free(FSOUND_SAMPLE* sptr)
{
    for (Channel& c : s_Channels)
    {
        if (c.sample == sptr)
        {
            MIX_StopTrack(c.track, 0);
            MIX_SetTrackAudio(c.track, NULL);
            c.sample = NULL;
        }
    }

    MIX_DestroyAudio(sptr->audio);
    delete sptr;
}

signed char FSOUND_Sample_GetDefaults(FSOUND_SAMPLE* sptr, int* deffreq, int* defvol, int* defpan, int* defpri)
{
    if (deffreq)
    {
        *deffreq = sptr->freq;
    }
    if (defvol)
    {
        *defvol = 255;
    }
    if (defpan)
    {
        *defpan = 128;
    }
    if (defpri)
    {
        *defpri = 255;
    }
    return 1;
}

int FSOUND_PlaySound3DAttrib(int channel, FSOUND_SAMPLE* sptr, int freq, int vol, int pan,
                             float* pos, float* vel)
{
    if (!s_Mixer || !sptr)
    {
        return -1;
    }

    if (channel == FSOUND_FREE)
    {
        auto it = std::find_if(s_Channels.begin(), s_Channels.end(), [](const Channel & c)
        {
            return !MIX_TrackPlaying(c.track);
        });
        if (it == s_Channels.end())
        {
            return -1;
        }
        channel = (int)(it - s_Channels.begin());
    }
    if (channel < 0 || channel >= (int)s_Channels.size())
    {
        return -1;
    }

    Channel& c = s_Channels[channel];
    c.sample = sptr;
    MIX_SetTrackAudio(c.track, sptr->audio);
    MIX_SetTrackFrequencyRatio(c.track, freq < 0 ? 1.0f : (float)freq / sptr->freq);
    MIX_SetTrackGain(c.track, (vol < 0 ? 1.0f : vol / 255.0f) * s_SampleGain);

    // Balance: centre plays both sides at full gain
    float p = pan < 0 ? 0.5f : pan / 255.0f;
    MIX_StereoGains gains = {std::min(1.0f, 2.0f * (1.0f - p)), std::min(1.0f, 2.0f * p)};
    MIX_SetTrackStereo(c.track, &gains);

    MIX_PlayTrack(c.track, 0);
    return channel;
}

signed char FSOUND_SetFrequency(int channel, int freq)
{
    for (int i = 0; i < (int)s_Channels.size(); i++)
    {
        Channel& c = s_Channels[i];
        if ((channel == FSOUND_ALL || channel == i) && c.sample)
        {
            MIX_SetTrackFrequencyRatio(c.track, (float)freq / c.sample->freq);
        }
    }
    return 1;
}

FSOUND_SAMPLE* FSOUND_GetCurrentSample(int channel)
{
    if (channel < 0 || channel >= (int)s_Channels.size() || !MIX_TrackPlaying(s_Channels[channel].track))
    {
        return NULL;
    }
    return s_Channels[channel].sample;
}

FMUSIC_MODULE* FMUSIC_LoadSong(const char* name)
{
    if (!s_Mixer)
    {
        return NULL;
    }

    MIX_Audio* audio = MIX_LoadAudio(s_Mixer, ResolvePath(name).c_str(), false);
    if (!audio)
    {
        return NULL;
    }

    MIX_Track* track = MIX_CreateTrack(s_Mixer);
    MIX_SetTrackAudio(track, audio);
    return new FMUSIC_MODULE{audio, track};
}

signed char FMUSIC_FreeSong(FMUSIC_MODULE* mod)
{
    if (!mod)
    {
        return 0;
    }
    MIX_DestroyTrack(mod->track);
    MIX_DestroyAudio(mod->audio);
    delete mod;
    return 1;
}

signed char FMUSIC_PlaySong(FMUSIC_MODULE* mod)
{
    return mod && MIX_PlayTrack(mod->track, 0);
}

signed char FMUSIC_StopSong(FMUSIC_MODULE* mod)
{
    return mod && MIX_StopTrack(mod->track, 0);
}

signed char FMUSIC_IsFinished(FMUSIC_MODULE* mod)
{
    return !mod || !MIX_TrackPlaying(mod->track);
}

signed char FMUSIC_SetMasterVolume(FMUSIC_MODULE* mod, int volume)
{
    return mod && MIX_SetTrackGain(mod->track, volume / 256.0f * s_MusicGain);
}
