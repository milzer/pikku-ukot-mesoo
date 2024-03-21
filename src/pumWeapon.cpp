#include "pumWeapon.h"
#include "pumPlayer.h"

CPumWeapon::CPumWeapon(const char *n)
{
    m_Name = new char[ strlen(n) + 1 ];
    strcpy(m_Name, n);

    SetDefault();
}

CPumWeapon::CPumWeapon()
{
    m_Name = NULL;

    SetDefault();
}

CPumWeapon::~CPumWeapon()
{
    if (m_Name != NULL)
        delete[] m_Name;
}

void CPumWeapon::SetDefault(const char *n)
{
    m_Name = new char[ strlen(n) + 1 ];
    strcpy(m_Name, n);

    m_Frag = NULL;

    m_Velocity          = 2.0f * TimeFactor;
    m_VelSpread         = 0.0f;
    m_Bounce            = 0.0f;
    m_Blow              = 0.0f;
    m_WeightModifier    = 1.0f;
    m_Seeker            = 0.0f;
    m_FrictionX         = 1.0f;
    m_FrictionY         = 1.0f;
    m_Accuracy          = 0.0f;
    m_BlowTimer         = 0.0f;
    m_SeekTimer         = 0.0f;

    m_Flags             = 0;
    m_SoundFireId[0]    = 18;
    m_SoundActivId[0]   = -1;
    m_SoundWallId[0]    = -1;
    m_SoundFireId[1]    = 18;
    m_SoundActivId[1]   = -1;
    m_SoundWallId[1]    = -1;
    m_Count             = 1;
    m_Damage            = 1;
    m_Clip              = 30;
    m_ReloadTime        = 1.0f;
    m_Firerate          = 0.5f;
    m_BounceCount       = 0;
    m_Col               = 0x00ffffff;
    m_Fragments         = 0;
}

void CPumWeapon::SetDefault()
{
    m_Frag = NULL;

    m_Velocity          = 2.0f * TimeFactor;
    m_VelSpread         = 0.0f;
    m_Bounce            = 0.0f;
    m_Blow              = 0.0f;
    m_WeightModifier    = 1.0f;
    m_Seeker            = 0.0f;
    m_FrictionX         = 1.0f;
    m_FrictionY         = 1.0f;
    m_Accuracy          = 0.0f;
    m_BlowTimer         = 0.0f;
    m_SeekTimer         = 0.0f;

    m_Flags             = 0;
    m_SoundFireId[0]    = 18;
    m_SoundActivId[0]   = -1;
    m_SoundWallId[0]    = -1;
    m_SoundFireId[1]    = 18;
    m_SoundActivId[1]   = -1;
    m_SoundWallId[1]    = -1;
    m_Count             = 1;
    m_Damage            = 1;
    m_Clip              = 30;
    m_ReloadTime        = 1.0f;
    m_Firerate          = 0.5f;
    m_BounceCount       = 0;
    m_Col               = 0x00ffffff;
    m_Fragments         = 0;
}

void CPumBullet::Activate(float frameTime)
{
    int             i;
    CDieselVector2  tmpvec;
    float           l,
    invl;

    tmpvec.Set(0.0f, 0.0f);
    m_SP->Sound->PlaySample(m_SoundActivId[0], m_SoundActivId[1], m_iXpos);

    if (m_Blow > 19.0f)
        m_Blow = 19.0f;

    if (m_Flags & WPN_EFF_FIRE) {
        SpawnParticles(_PRT_FIREEMIT, m_fXpos - (m_Vec.x / TimeFactor), m_fYpos - (m_Vec.y / TimeFactor), 192.0f, 100.0f, 0.1f, TimeFactor, 0.0f, (int)m_Blow * 2, 300 + RandInt(0, 200), 0, m_SP);
    } else if (m_Blow > 0.0f) {

        for (l = 1.8f; l < 20.0f; l += 6.0f)
            if (m_Blow > l) m_SP->Sound->PlaySample(1, 7, m_iXpos);

        SpawnParticles(_PRT_EXPLOSION, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.1f, m_Blow * TimeFactor, 0, (int)m_Blow * 10, 255, 0, m_SP);
        SpawnParticles(_PRT_FASTSPARK, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.1f, m_Blow * TimeFactor, 0, (int)m_Blow * 5, 255, 0, m_SP);
        SpawnParticles(_PRT_SPARK, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.1f, m_Blow * 0.7f * TimeFactor, 0, (int)m_Blow * 2, 255, 0, m_SP);
        if (m_Blow > 1.9)
            SpawnParticles(_PRT_DEBRIS, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.1f, m_Blow * 0.6f * TimeFactor, 0, (int)m_Blow * 2, 255, 0, m_SP);

        for (i = 0; i < m_SP->Options->Players; i++) {
            if (m_SP->Player[i].m_Alive == TRUE) {
                tmpvec.Set(m_SP->Player[i].m_fXpos - m_fXpos, m_SP->Player[i].m_fYpos - m_fYpos);
                l = tmpvec.Length();

                if (l < 1.0f)
                    invl = 1.0f;
                else
                    invl = 1.0f / l;

                // (1/etäisyys*21)*(blow/20)*(blow)

                tmpvec.Normalize();
                tmpvec.Mul(invl * TimeFactor / 2);

                if (l < 40)
                    m_SP->Player[i].m_Vec.Add(tmpvec);

                if (l < 64)
                    m_SP->Player[i].Damage((int)((invl * 21.0f) *(m_Blow / 20.0f) *(m_Blow * 2)), &tmpvec);
            }
        }
    }

    for (i = 0; i < m_Fragments; i++)
        m_SP->Weapon->ShootB(m_fXpos, m_fYpos, (float)(rand()&255), NULL, m_Frag);

    Kill();
}

void CPumBullet::GetWeaponInfo(CPumWeapon *w)
{
    m_Velocity          = w->m_Velocity;
    m_VelSpread         = w->m_VelSpread;
    m_Bounce            = w->m_Bounce;
    m_Blow              = w->m_Blow;
    m_WeightModifier    = w->m_WeightModifier;
    m_Seeker            = w->m_Seeker;
    m_FrictionX         = w->m_FrictionX;
    m_FrictionY         = w->m_FrictionY;
    m_Accuracy          = w->m_Accuracy;
    m_Count             = w->m_Count;
    m_Damage            = w->m_Damage;
    m_Clip              = w->m_Clip;
    m_ReloadTime        = w->m_ReloadTime;
    m_Firerate          = w->m_Firerate;
    m_BounceCount       = w->m_BounceCount;
    m_BlowTimer         = w->m_BlowTimer;
    m_SeekTimer         = w->m_SeekTimer;
    m_Col               = w->m_Col;
    m_Frag              = w->m_Frag;
    m_Flags             = w->m_Flags;
    m_Fragments         = w->m_Fragments;

    m_SoundFireId[0]    = w->m_SoundFireId[0];
    m_SoundActivId[0]   = w->m_SoundActivId[0];
    m_SoundWallId[0]    = w->m_SoundWallId[0];
    m_SoundFireId[1]    = w->m_SoundFireId[1];
    m_SoundActivId[1]   = w->m_SoundActivId[1];
    m_SoundWallId[1]    = w->m_SoundWallId[1];
}

int CPumBullet::Update(float frameTime)
{
    CHAR    pix;
    float   fXOpos = m_fXpos,
                     fYOpos = m_fYpos;

    m_TTL--;

    m_Vec.y += m_SP->Options->Gravity * m_WeightModifier * frameTime;
    m_Vec.x *= m_FrictionX;
    m_Vec.y *= m_FrictionY;

    m_fXpos += m_Vec.x * frameTime;
    m_fYpos += m_Vec.y * frameTime;

    RoundToInt(&m_iXpos, m_fXpos);
    RoundToInt(&m_iYpos, m_fYpos);

    if (m_iXpos < ScreenWidth - 1 && m_iXpos > 0 && m_iYpos < ScreenHeight - 1 && m_iYpos > 0) {
        m_Onscreen = 1;
        pix = m_SP->Level->m_szMask[m_iYpos * ScreenWidth + m_iXpos];
    } else m_Onscreen = 0;

    m_TTL *= m_Onscreen;

    if (m_TTL < 1)
        return(0);

    if (m_BlowTimer > 0.0f) {
        m_BlowTimer -= frameTime;
        if (m_BlowTimer < 0.0f) {
            Activate(frameTime);
            return(0);
        }
    }

    if (m_Seeker > 0.0f) {
        m_SeekTimer -= frameTime;

        if (m_Flags & WPN_SEEKER_ACTIVE) {
            if (m_SeekTimer < 0.0f) {
                float           l = 999999.0f,
                                    ltmp;
                int             i;
                CDieselVector2  tmpvec;
                tmpvec.Set(0.0f, 0.0f);

                for (i = 0; i < m_SP->Options->Players; i++) {
                    if (m_SP->Player[i].m_Alive == TRUE) {
                        ltmp = Distance(m_fXpos, m_fYpos, m_SP->Player[i].m_fXpos, m_SP->Player[i].m_fYpos);

                        if (ltmp < l) {
                            tmpvec.Set(m_SP->Player[i].m_fXpos - m_fXpos, m_SP->Player[i].m_fYpos - m_fYpos);
                            l = ltmp;
                        }
                    }
                }

                tmpvec.Normalize();
                tmpvec.Mul(TimeFactor * m_Seeker);

                m_Vec.Add(tmpvec);

                m_SeekTimer = 0.05f;
            }
        } else {
            if (m_SeekTimer < 0.0f) {
                m_SeekTimer = 0.07f;
                m_Flags |= WPN_SEEKER_ACTIVE;
            }
        }
    }

    /*
    #define WPN_EFF_SPARKBACK   0x0100
    #define WPN_EFF_SPARKS      0x0400
    */

    if (m_Flags & WPN_EFF_SPARKBACK)
        SpawnParticles(_PRT_SPARK, m_fXpos, m_fYpos, Angle(m_Vec.x, m_Vec.y, 0, 0), 32.0f, 0.5f * TimeFactor, 1.1f * TimeFactor, 0, rand()&3, 255, 0, m_SP);
    else if (m_Flags & WPN_EFF_SPARKS)
        SpawnParticles(_PRT_SPARK, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.5f * TimeFactor, 1.1f * TimeFactor, 0, rand()&1, 255, 0, m_SP);


    if (m_Onscreen) {
        if (pix) {
            if (!(rand()&3)) m_SP->Sound->PlaySample(m_SoundWallId[0], m_SoundWallId[1], m_iXpos);

            if (m_Flags & WPN_EFF_WALLSPARK) {
                SpawnParticles(_PRT_FASTSPARK, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.1f, TimeFactor, 0, 7, 255, 0, m_SP);
                if (!(rand()&7)) SpawnParticles(_PRT_SPARK, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.1f, 0.7f * TimeFactor, 0, 1, 255, 0, m_SP);
            }

            if (m_Bounce > 0.0f) {
                m_BounceCount--;

                if (m_BounceCount == 0) {
                    Activate(frameTime);
                    return(0);
                }

                WallBounce(&m_Vec, m_iXpos, m_iYpos, m_SP->Level->m_szMask, frameTime);
                m_Vec.Mul(m_Bounce);
            } else
                Activate(frameTime);

            m_fXpos = fXOpos;
            m_fYpos = fYOpos;
            RoundToInt(&m_iXpos, m_fXpos);
            RoundToInt(&m_iYpos, m_fYpos);
        }
    }

    return(m_TTL);
}

void CPumBullet::Kill()
{
    m_Vec.Set(0.0f, 0.0f);

    m_Onscreen = 0;

    m_fXpos = 0.0f;
    m_fYpos = 0.0f;

    m_TTL = 0;
    m_iXpos = 0;
    m_iYpos = 0;

    SetDefault();
}

void CPumBullet::Draw(DWORD *pSurface)
{
    if (m_iXpos < 1) return;
    if (m_iXpos > ScreenWidth - 2) return;
    if (m_iYpos < 1) return;
    if (m_iYpos > ScreenHeight - 2) return;

    pSurface[ m_iYpos * ScreenWidth + m_iXpos ] = m_Col;
}

void CPumBullet::Create(float x, float y, float angle, CPumWeapon *w, CPumSysPointers *sp)
{
    int     a;
    float   v;

    if (x < 1.0f) return;
    if (x > 318.0f) return;
    if (y < 1.0f) return;
    if (y > 198.0f) return;

    RoundToInt(&a, angle);
    a &= 255;

    GetWeaponInfo(w);

    m_TTL = 2000;

    m_fXpos = x;
    m_fYpos = y;

    v = (m_Velocity + RandFloat(-m_VelSpread, m_VelSpread)) * TimeFactor;

    m_Vec.Set(cosinus[a] * v,
              sinus[a] * v);

    m_SP = sp;
}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

CPumWeapon *CPumWeaponSystem::FindWeapon(const char *n)
{
    int i;

    for (i = 0; i < m_MaxW; i++) {
        if (&m_Weapons[i] != NULL) {
            if (m_Weapons[i].m_Name != NULL) {
                if (!stricmp(m_Weapons[i].m_Name, n))
                    return(&m_Weapons[i]);
            }

        }
    }

    return(NULL);
}

void CPumWeaponSystem::LoadWeapons()
{
    int     currentwpn = -1;
    FILE    *fp;
    char    *str = new char[128];
    BOOL    comment = FALSE;

    m_MaxW = 0;

    fp = fopen("aseet.puo", "rt");
    if (!fp) {
        delete[] str;
        return;
    }

    do {
        fscanf(fp, "%s", str);

        if (comment == TRUE) {
            if (!stricmp(str, "-->"))
                comment = FALSE;
        } else {
            if (!stricmp(str, "<!--"))
                comment = TRUE;

            if (!stricmp(str, "<ase:>"))
                m_MaxW++;

            if (!stricmp(str, "<efu:>"))
                m_MaxW++;
        }

    } while (!feof(fp));

    if (m_MaxW < 1)
        return;

    m_Weapons = new CPumWeapon[ m_MaxW ];

    fseek(fp, 0, SEEK_SET);

    do {

        fscanf(fp, "%s", str);

        if (comment == TRUE) {
            if (!stricmp(str, "-->"))
                comment = FALSE;
        } else {
            if (!stricmp(str, "<!--")) {
                comment = TRUE;
            }

            if (!stricmp(str, "<ase:>")) {
                currentwpn++;
                fscanf(fp, "%s", str);
                m_Weapons[ currentwpn ].SetDefault(str);

            }

            if (!stricmp(str, "<efu:>")) {
                currentwpn++;
                fscanf(fp, "%s", str);
                m_Weapons[ currentwpn ].SetDefault(str);
                m_Weapons[ currentwpn ].m_Flags |= WPN_EFFECT;
            }

            if (!stricmp(str, "nakymaton")) {
                m_Weapons[ currentwpn ].m_Flags |= WPN_INVISIBLE;
            }

            if (!stricmp(str, "kierra-pelaaja")) {
                m_Weapons[ currentwpn ].m_Flags |= WPN_IGNORE_PLR;
            }

            if (!stricmp(str, "vahinko")) {
                fscanf(fp, "%d", &m_Weapons[ currentwpn ].m_Damage);
            }

            if (!stricmp(str, "lahtonopeus")) {
                fscanf(fp, "%f %f", &m_Weapons[ currentwpn ].m_Velocity, &m_Weapons[ currentwpn ].m_VelSpread);
            }

            if (!stricmp(str, "tulinopeus")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_Firerate);
            }

            if (!stricmp(str, "lipas")) {
                fscanf(fp, "%d", &m_Weapons[ currentwpn ].m_Clip);
            }

            if (!stricmp(str, "latausaika")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_ReloadTime);
            }

            if (!stricmp(str, "kumi")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_Bounce);
            }

            if (!stricmp(str, "kumiluku")) {
                fscanf(fp, "%d", &m_Weapons[ currentwpn ].m_BounceCount);
            }

            if (!stricmp(str, "rajahdys")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_Blow);
            }

            if (!stricmp(str, "rajahdys-ajastus")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_BlowTimer);
            }

            if (!stricmp(str, "efekti")) {
                fscanf(fp, "%s", str);

                if (!stricmp(str, "kipinasuihku"))
                    m_Weapons[ currentwpn ].m_Flags |= WPN_EFF_SPARKBACK;

                if (!stricmp(str, "kipinaroiske"))
                    m_Weapons[ currentwpn ].m_Flags |= WPN_EFF_SPARKS;

                if (!stricmp(str, "seinakipina"))
                    m_Weapons[ currentwpn ].m_Flags |= WPN_EFF_WALLSPARK;
            }

            if (!stricmp(str, "tuli")) {
                m_Weapons[ currentwpn ].m_Flags |= WPN_EFF_FIRE;
            }

            if (!stricmp(str, "painokerroin")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_WeightModifier);
            }

            if (!stricmp(str, "sirpaleita")) {
                fscanf(fp, "%d %s", &m_Weapons[ currentwpn ].m_Fragments, str);

                m_Weapons[ currentwpn ].m_Frag = FindWeapon(str);
            }

            if (!stricmp(str, "haku")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_Seeker);
            }

            if (!stricmp(str, "haku-ajastus")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_SeekTimer);
            }

            if (!stricmp(str, "kitka")) {
                fscanf(fp, "%f %f", &m_Weapons[ currentwpn ].m_FrictionX, &m_Weapons[ currentwpn ].m_FrictionY);
            }

            if (!stricmp(str, "vari")) {
                int r, g, b;

                fscanf(fp, "%d %d %d", &r, &g, &b);
                m_Weapons[ currentwpn ].m_Col = (r << 16) | (g << 8) | b;
            }

            if (!stricmp(str, "tarkkuus")) {
                fscanf(fp, "%f", &m_Weapons[ currentwpn ].m_Accuracy);
            }

            if (!stricmp(str, "lukumaara")) {
                fscanf(fp, "%d", &m_Weapons[ currentwpn ].m_Count);
            }

            if (!stricmp(str, "aani-laukaus-id")) {
                fscanf(fp, "%d %d", &m_Weapons[ currentwpn ].m_SoundFireId[0], &m_Weapons[ currentwpn ].m_SoundFireId[1]);
            }

            if (!stricmp(str, "aani-aktiv-id")) {
                fscanf(fp, "%d %d", &m_Weapons[ currentwpn ].m_SoundActivId[0], &m_Weapons[ currentwpn ].m_SoundActivId[1]);
            }

            if (!stricmp(str, "aani-seina-id")) {
                fscanf(fp, "%d %d", &m_Weapons[ currentwpn ].m_SoundWallId[0], &m_Weapons[ currentwpn ].m_SoundWallId[1]);
            }
        }

    } while (!feof(fp));

    fclose(fp);
    delete[] str;
}


void CPumWeaponSystem::ShootB(float x, float y, float a, CPumPlayer *p, CPumWeapon *w)
{
    int i;

    if (x < 1.0f) return;
    if (x > 318.0f) return;
    if (y < 1.0f) return;
    if (y > 198.0f) return;

    m_SP->Sound->PlaySample(w->m_SoundFireId[0], w->m_SoundFireId[1], (int)x);

    for (i = 0; i < w->m_Count; i++) {
        if (p == NULL)
            a = a + RandFloat(-w->m_Accuracy, w->m_Accuracy);
        else
            a = a + RandFloat(-(p->m_AccuracyMod + w->m_Accuracy), p->m_AccuracyMod + w->m_Accuracy);

        m_Bullets[ m_CurrentB++ ].Create(x, y, a, w, m_SP);

        if (m_CurrentB >= m_MaxB)
            m_CurrentB = 0;
    }
}

void CPumWeaponSystem::UpdateB(float frameTime)
{
    int i, j;

    for (i = 0; i < m_MaxB; i++) {
        if (m_Bullets[i].m_TTL > 0) {
            if (m_Bullets[i].Update(frameTime)) {
                if (!(m_Bullets[i].m_Flags & WPN_IGNORE_PLR)) {
                    for (j = 0; j < m_SP->Options->Players; j++) {
                        if (m_SP->Player[j].HitB(&m_Bullets[i]))
                            m_Bullets[i].Activate(frameTime);
                    }
                }
            }
        } else
            m_Bullets[i].Kill();
    }
}

void CPumWeaponSystem::DrawB(DWORD *pSurface)
{
    int i;

    for (i = 0; i < m_MaxB; i++) {
        if (m_Bullets[i].m_TTL > 0 && !(m_Bullets[i].m_Flags & WPN_INVISIBLE))
            m_Bullets[i].Draw(pSurface);
    }
}

void CPumWeaponSystem::Reset()
{
    int i;

    for (i = 0; i < m_MaxB; i++)
        m_Bullets[i].Kill();
}

CPumWeaponSystem::CPumWeaponSystem(const int i)
{
    m_Bullets = NULL;
    m_Bullets = new CPumBullet[ i ];
    if (m_Bullets != NULL) {
        m_MaxB = i;
        m_CurrentB = 0;
    }

    m_Weapons = new CPumWeapon;
}

CPumWeaponSystem::~CPumWeaponSystem()
{
    if (m_Bullets != NULL)
        delete[] m_Bullets;

    delete[] m_Weapons;
}