#include "pumSound.h"
#include <io.h>

extern float globalMatrixTime;

CPumSoundSystem *SoundSystem;

CPumSoundSystem::CPumSoundSystem(SDE_PUMOPTIONS *opt)
{
    int i;

    m_SongList = NULL;
    m_PlayList = NULL;

    for (i = 0; i < MAX_SAMPLES; i++)
        m_Sample[i] = NULL;

    m_Music = NULL;
    m_MusicVol = opt->MusicVolume;

    FSOUND_SetOutput(opt->SoundOutput);
    FSOUND_SetDriver(0);
    FSOUND_SetMixer(FSOUND_MIXER_QUALITY_AUTODETECT);

    FSOUND_Init(opt->SoundFrequency, opt->SoundChannels, 0);
}

CPumSoundSystem::~CPumSoundSystem()
{
    int i;

    for (i = 0; i < MAX_SAMPLES; i++) {
        if (m_Sample[i])
            FSOUND_Sample_Free(m_Sample[i]);
    }

    if (m_Music) {
        FMUSIC_StopSong(m_Music);
        FMUSIC_FreeSong(m_Music);
    }

    for (i = 0; i < m_SongCount; i++)
        if (m_SongList[i])
            delete[] m_SongList[i];

    if (m_SongList)
        delete[] m_SongList;

    if (m_PlayList)
        delete[] m_PlayList;

    FSOUND_Close();
}

void CPumSoundSystem::SlowTime(int defaultfreq)
{
    FSOUND_SetFrequency(FSOUND_ALL, (int)((float)defaultfreq * globalMatrixTime));
}

void CPumSoundSystem::UpdateMusic()
{
    if (m_SongCount < 0) return;
    if (m_MusicVol < 1) return;

    int i, smallest = 9999, largest = -1;

    if (m_Music == NULL || FMUSIC_IsFinished(m_Music) == TRUE) {
        for (i = 0; i < m_SongCount; i++) {
            if (m_PlayList[i] < smallest)
                smallest = m_PlayList[i];

            if (m_PlayList[i] > largest)
                largest = m_PlayList[i];
        }

        if (largest == smallest) {
            i = rand() % m_SongCount;
        } else {
            i = rand() % m_SongCount;

            while (m_PlayList[i] != smallest)
                i = rand() % m_SongCount;
        }

        PlayMusic(m_SongList[i]);
        m_PlayList[i]++;
    }
}

void CPumSoundSystem::LoadMusic(const char *f)
{
    if (m_SongCount < 0) return;
    if (m_MusicVol < 1) return;

    if (m_Music)
        FMUSIC_FreeSong(m_Music);

    m_Music = FMUSIC_LoadSong(f);
}

void CPumSoundSystem::PlayMusic()
{
    if (m_SongCount < 0) return;
    if (m_MusicVol < 1) return;

    if (m_Music) {
        FMUSIC_SetMasterVolume(m_Music, m_MusicVol);
        FMUSIC_PlaySong(m_Music);
    }
}

void CPumSoundSystem::PlayMusic(const char *f)
{
    if (m_SongCount < 0) return;
    if (m_MusicVol < 1) return;

    LoadMusic(f);
    PlayMusic();
}

void CPumSoundSystem::StopMusic()
{
    if (m_Music)
        FMUSIC_StopSong(m_Music);
}

void CPumSoundSystem::LoadSample(int id, const char *f)
{
    m_Sample[ id ] = FSOUND_Sample_Load(FSOUND_FREE, f, FSOUND_2D, 0);
}

void CPumSoundSystem::PlaySample(int id, int x)
{
    if (id < 0) return;
    if (x < 0) return;
    if (x > 319) return;
    x = (int)((float)x * 0.797f);

//  FSOUND_PlaySound3DAttrib(FSOUND_FREE, m_Sample[id], -1, -1, x, NULL, NULL);
}

void CPumSoundSystem::PlaySample(int id1, int id2, int x)
{
    if (id1 < 0) return;
    if (id2 < 0) return;
    if (x < 0) return;
    if (x > 319) return;
    x = (int)((float)x * 0.797f);

//  FSOUND_PlaySound3DAttrib(FSOUND_FREE, m_Sample[ RandInt( id1, id2 ) ], -1, -1, x, NULL, NULL);
}

void CPumSoundSystem::LoadSoundScript(const char *f)
{
    int     c = 0,
                i;
    char    *tmp;
    FILE    *fp;

    fp = fopen(f, "rt");
    if (!fp) return;

    tmp = new char[256];

    while (!feof(fp)) {
        fscanf(fp, "%s %d\n", tmp, &i);
        LoadSample(i, tmp);

        c++;
    }

    fclose(fp);

    delete[] tmp;
}

void CPumSoundSystem::GetMusics()
{
    struct  _finddata_t fblock;
    long    h = 0,
                i = 0;

    h = _findfirst("musiq\\*.mus", &fblock);

    if (h == -1)
        return;

    m_SongCount = 1;

    while (_findnext(h, &fblock) == 0)
        m_SongCount++;

    _findclose(h);

    m_SongList = new char*[m_SongCount];
    m_PlayList = new int[m_SongCount];

    for (i = 0; i < m_SongCount; i++)
        m_PlayList[ i ] = 0;

    i = 0;

    h = _findfirst("musiq\\*.mus", &fblock);

    m_SongList[ i ] = new char[ strlen(fblock.name) + 7 ];
    strcpy(m_SongList[ i ], "musiq\\");
    strcat(m_SongList[ i ], fblock.name);
    i++;

    while (_findnext(h, &fblock) == 0) {
        m_SongList[ i ] = new char[ strlen(fblock.name) + 7 ];
        strcpy(m_SongList[ i ], "musiq\\");
        strcat(m_SongList[ i ], fblock.name);
        i++;
    }

    _findclose(h);

    return;
}

void CPumSoundSystem::UpdateVolume()
{
    FMUSIC_SetMasterVolume(m_Music, m_MusicVol);
}