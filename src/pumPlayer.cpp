#include "pumPlayer.h"

extern float globalMatrixTime;

CPumPlayer::CPumPlayer()
{
    Reset();
}

CPumPlayer::~CPumPlayer()
{
    int i;

    for (i = 0; i < PLR_FRAMES; i++)
        delete[] m_Gfx[i];
}

void CPumPlayer::SetColor(DWORD c)
{
    int i;

    for (i = 0; i < PLR_FRAMES; i++) {
        m_Gfx[i] = new DWORD[20];
        memset(m_Gfx[i], 0, 20*sizeof(DWORD));
    }

    // left 1

    m_Gfx[0][2] = 0x00ffd4bc; // head
    m_Gfx[0][5] = 0x00bcbcbc; // gun
    m_Gfx[0][6] = 0x00bcbcbc; // gun
    m_Gfx[0][7] = c; // torso
    m_Gfx[0][8] = c; // hand
    m_Gfx[0][12] = c; // gut
    m_Gfx[0][16] = c; // leg
    m_Gfx[0][18] = c; // leg
    m_Gfx[0][17] = 0x00000000; // dick :)

    // left 2

    m_Gfx[1][2] = 0x00ffd4bc; // head
    m_Gfx[1][5] = 0x00bcbcbc; // gun
    m_Gfx[1][6] = 0x00bcbcbc; // gun
    m_Gfx[1][7] = c; // torso
    m_Gfx[1][8] = c; // hand
    m_Gfx[1][12] = c; // gut
    m_Gfx[1][17] = c; // legs

    // right 1

    m_Gfx[2][2] = 0x00ffd4bc; // head
    m_Gfx[2][8] = 0x00bcbcbc; // gun
    m_Gfx[2][9] = 0x00bcbcbc; // gun
    m_Gfx[2][7] = c; // torso
    m_Gfx[2][6] = c; // hand
    m_Gfx[2][12] = c; // gut
    m_Gfx[2][16] = c; // leg
    m_Gfx[2][18] = c; // leg
    m_Gfx[2][17] = 0x00000000; // dick :)

    // right 2

    m_Gfx[3][2] = 0x00ffd4bc; // head
    m_Gfx[3][8] = 0x00bcbcbc; // gun
    m_Gfx[3][9] = 0x00bcbcbc; // gun
    m_Gfx[3][7] = c; // torso
    m_Gfx[3][6] = c; // hand
    m_Gfx[3][12] = c; // gut
    m_Gfx[3][17] = c; // legs

    Reset();
}

void CPumPlayer::Reset()
{
    m_Frame = 0;
    m_iXpos = 0;
    m_iYpos = 0;
    m_iAngle = 128;
    m_AccuracyMod = 0.0f;
    m_WalkSpeed = 0.8f * TimeFactor;
    m_JumpPower = 1.6f * TimeFactor;
    m_Vec.Set(0.0f, 0.0f);
    m_Health = 255;
    m_Alive = TRUE;
    m_FireDelay = -1.0f;
    m_CurrentWpn = 0;
    m_WeaponShow = 0.4f;
}

void CPumPlayer::ReadKeys(CDieselInputKeyboard *pKbd)
{
    m_KeyBuffer[ PLR_C_LEFT ] = pKbd->GetButton(m_Keys[PLR_C_LEFT], NULL);
    m_KeyBuffer[ PLR_C_RIGHT ] = pKbd->GetButton(m_Keys[PLR_C_RIGHT], NULL);
    m_KeyBuffer[ PLR_C_UP ] = pKbd->GetButton(m_Keys[PLR_C_UP], NULL);
    m_KeyBuffer[ PLR_C_DOWN ] = pKbd->GetButton(m_Keys[PLR_C_DOWN], NULL);
    m_KeyBuffer[ PLR_C_CHANGE ] = pKbd->GetButton(m_Keys[PLR_C_CHANGE], NULL);
    m_KeyBuffer[ PLR_C_JUMP ] = pKbd->GetButton(m_Keys[PLR_C_JUMP], NULL);
    m_KeyBuffer[ PLR_C_SHOOT ] = pKbd->GetButton(m_Keys[PLR_C_SHOOT], NULL);
    m_KeyBuffer[ PLR_C_GRAPPLE ] = pKbd->GetButton(m_Keys[PLR_C_GRAPPLE], NULL);
}

void CPumPlayer::UpdateControls(DWORD *pSrf)
{
    int h = 0;

    if (m_Alive == FALSE) return;

    if (m_Vec.x > -m_WalkSpeed && m_KeyBuffer[ PLR_C_LEFT ] == TRUE && m_KeyBuffer[ PLR_C_RIGHT ] == FALSE) {
        if (m_Frame > 1) {
            if (m_iAngle < 64) m_iAngle = 128 - m_iAngle;
            else m_iAngle = 192 - (m_iAngle - 192);
        }

        if (m_Vec.x > -m_WalkSpeed) {
            m_Vec.x -= m_WalkSpeed / 4.0f;
            if (m_Vec.x < -m_WalkSpeed)
                m_Vec.x = -m_WalkSpeed;
        }

        m_Frame = (m_Frame + 1) & 1;
    }

    if (m_Vec.x < m_WalkSpeed && m_KeyBuffer[ PLR_C_RIGHT ] == TRUE && m_KeyBuffer[ PLR_C_LEFT ] == FALSE) {
        if (m_Frame < 2) m_iAngle = 192 + (192 - m_iAngle);

        if (m_Vec.x < m_WalkSpeed) {
            m_Vec.x += m_WalkSpeed / 4.0f;
            if (m_Vec.x > m_WalkSpeed)
                m_Vec.x = m_WalkSpeed;
        }
        m_Frame = 2 + ((m_Frame + 1) & 1);
    }

    if (m_KeyBuffer[ PLR_C_UP ] == TRUE) {
        if (m_Frame < 2) { // left
            m_iAngle += 2;
            if (m_iAngle > 192) m_iAngle = 192;
        } else { // right
            m_iAngle -= 2;
            if (m_iAngle < 192 && m_iAngle > 64) m_iAngle = 192;
        }
    }

    if (m_KeyBuffer[ PLR_C_DOWN ] == TRUE) {
        if (m_Frame < 2) { // left
            m_iAngle -= 2;
            if (m_iAngle < 64) m_iAngle = 64;
        } else { // right
            m_iAngle += 2;
            if (m_iAngle > 64 && m_iAngle < 192) m_iAngle = 64;
        }
    }

    if (m_KeyBuffer[ PLR_C_CHANGE ] == TRUE && m_FireDelay < 0.0f) {
        m_WeaponShow = 0.4f;

        m_SP->Sound->PlaySample(12, m_iXpos);
        m_FireDelay = 0.3f;

        ++m_CurrentWpn %= m_SP->Weapon->m_MaxW;

        while (m_SP->Weapon->m_Weapons[ m_CurrentWpn ].m_Flags & WPN_EFFECT)
            ++m_CurrentWpn %= m_SP->Weapon->m_MaxW;
    }

    if (m_KeyBuffer[ PLR_C_SHOOT ] == TRUE && m_FireDelay < 0.0f) {
        SpawnParticles(_PRT_FASTSPARK, cosinus[ m_iAngle ] * 2 + m_iXpos + (m_Vec.x * 0.02f) + 1, sinus[ m_iAngle ] * 2 + m_iYpos + (m_Vec.y * 0.02f) - 1, (float)m_iAngle, 14, 0.8f * TimeFactor, 2.2f * TimeFactor, 0.0f, 4, 255, (BYTE)0, m_SP);
        SpawnParticles(_PRT_FASTSPARK, cosinus[ m_iAngle ] * 3 + m_iXpos + (m_Vec.x * 0.02f) + 1, sinus[ m_iAngle ] * 3 + m_iYpos + (m_Vec.y * 0.02f) - 1, (float)m_iAngle + 40, 4, 0.2f * TimeFactor, 0.9f * TimeFactor, 0.0f, 4, 255, (BYTE)0, m_SP);
        SpawnParticles(_PRT_FASTSPARK, cosinus[ m_iAngle ] * 3 + m_iXpos + (m_Vec.x * 0.02f) + 1, sinus[ m_iAngle ] * 3 + m_iYpos + (m_Vec.y * 0.02f) - 1, (float)m_iAngle + 216, 4, 0.2f * TimeFactor, 0.9f * TimeFactor, 0.0f, 4, 255, (BYTE)0, m_SP);

        m_SP->Weapon->ShootB(
            cosinus[ m_iAngle ] * 5.0f + m_iXpos + 1,
            sinus[ m_iAngle ] * 5.0f + m_iYpos - 1,
            (float)m_iAngle,
            this,
            &m_SP->Weapon->m_Weapons[ m_CurrentWpn ]);

        m_FireDelay = m_SP->Weapon->m_Weapons[ m_CurrentWpn ].m_Firerate;
    }

    if (m_KeyBuffer[ PLR_C_JUMP ] == TRUE && CheckBHit(0, 1, pSrf))
        m_Vec.y = -m_JumpPower;
}

void CPumPlayer::Update(DWORD *pSrf, float frameTime)
{
    int             notx = 0;
    float           tmp;

    if (m_Alive == FALSE) {
        if (m_Health < 255) {
            m_Health++;
            return;
        } else {
            Reset();
            do {
                m_iXpos = RandInt(0, ScreenWidth);
                m_iYpos = RandInt(0, ScreenHeight);
                m_fXpos = (float)m_iXpos;
                m_fYpos = (float)m_iYpos;
            } while (CheckBHit(0.0f, 0.0f, pSrf));
        }
    }

    if (m_Health < 0) {
        SpawnParticles(_PRT_GIB, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.5f * TimeFactor, 1.0f * TimeFactor, 0, 9, 0, 195, m_SP);
        SpawnParticles(_PRT_GIB, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.0f, (-m_Health) / 6 * TimeFactor, 0, (-m_Health) / 5, 0, 195, m_SP);
        SpawnParticles(_PRT_BLOOD, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.0f, (-m_Health) / 16 * TimeFactor, 0, (-m_Health) * 12, 0, 195, m_SP);
        SpawnParticles(_PRT_BLOOD, m_fXpos, m_fYpos, 0.0f, 256.0f, 0.0f, TimeFactor, 0, 16, 0, 195, m_SP);

        if (m_Health < -70)
            globalMatrixTime = 0.02f;

        m_Alive = FALSE;
    }

    if (m_WalkSpeed < 0.8f * TimeFactor)
        m_WalkSpeed += 0.001f * TimeFactor;

    if (m_JumpPower < 1.6f * TimeFactor)
        m_JumpPower += 0.001f * TimeFactor;

    m_WeaponShow -= frameTime;

    while (m_SP->Weapon->m_Weapons[ m_CurrentWpn ].m_Flags & WPN_EFFECT)
        ++m_CurrentWpn %= m_SP->Weapon->m_MaxW;

    frameTime = Crop(frameTime, 0.00f, 0.05f);

    if (m_FireDelay > -0.01) m_FireDelay -= frameTime;
    m_iAngle &= 255;

    m_Vec.y += m_SP->Options->Gravity * frameTime;
    m_Vec.x *= 0.8f;

    if (CheckBHit(0, m_Vec.y*frameTime, pSrf)) {
        if (m_Vec.Length() > (2.6f * TimeFactor)) {
            SpawnParticles(_PRT_BLOOD, m_fXpos, m_fYpos + 1, 0, 256.0f, 0, (float)(m_Vec.y - (5.5*TimeFactor)), 1.0f, RoundDef((m_Vec.y*frameTime - 2)*32), 0, 0, m_SP);
            m_SP->Sound->PlaySample(10, m_iXpos);
            m_Health -= (int)(m_Vec.Length() / 20);
        }

        if (m_Vec.y < 0.0f) m_Vec.y = 0.0f;
        else {
            for (tmp = 0.0f; tmp < 4.0f; tmp += 0.25f) {
                if (!CheckBHit(0, m_Vec.y * frameTime - tmp, pSrf)) {
                    m_fYpos -= tmp;
                    m_Vec.y = 0;
                    tmp = 4.0f;
                    notx = 1;
                }
            }
        }
    }

    if (CheckBHit(m_Vec.x * frameTime, m_Vec.y * frameTime, pSrf))
        m_Vec.x = 0.0f;

    m_fXpos += m_Vec.x * frameTime;
    m_fYpos += m_Vec.y * frameTime;

    if (CheckBHit(0, 0, pSrf))
        EmergencyExit(pSrf);

    if (m_fXpos > (float)ScreenWidth) m_Health = -1;
    else if (m_fXpos < 0.0f) m_Health = -1;
    if (m_fYpos > (float)ScreenHeight) m_Health = -1;
    else if (m_fYpos < 0.0f) m_Health = -1;

    RoundToInt(&m_iXpos, m_fXpos);
    RoundToInt(&m_iYpos, m_fYpos);
}

void CPumPlayer::Position(float x, float y)
{
    m_fXpos = x;
    m_fYpos = y;

    m_Vec.Set(0.0f, 0.0f);

    RoundToInt(&m_iXpos, m_fXpos);
    RoundToInt(&m_iYpos, m_fYpos);
}

void CPumPlayer::SetKeys(BYTE _left, BYTE _right, BYTE _up, BYTE _down, BYTE _change, BYTE _jump, BYTE _shoot)
{
    m_Keys[PLR_C_LEFT] = _left;
    m_Keys[PLR_C_RIGHT] = _right;
    m_Keys[PLR_C_UP] = _up;
    m_Keys[PLR_C_DOWN] = _down;
    m_Keys[PLR_C_CHANGE] = _change;
    m_Keys[PLR_C_JUMP] = _jump;
    m_Keys[PLR_C_SHOOT] = _shoot;
}

void CPumPlayer::LoadKeys(int i)
{
    FILE *fp;

    switch (i) {
    case 1:
        fp = fopen("keys.1", "rb");
        break;
    case 2:
        fp = fopen("keys.2", "rb");
        break;
    case 3:
        fp = fopen("keys.3", "rb");
        break;
    }

    if (fp) {
        if (fread(m_Keys, 1, sizeof(m_Keys), fp) == sizeof(m_Keys)) {
            fclose(fp);
            return;
        } else
            fclose(fp);

    }

    switch (i) {
    case 1:
        SetKeys(DIK_LEFT, DIK_RIGHT, DIK_UP, DIK_DOWN, DIK_RSHIFT, DIK_RETURN, DIK_RCONTROL);
        break;
    case 2:
        SetKeys(DIK_S, DIK_F, DIK_E, DIK_D, DIK_LSHIFT, DIK_TAB, DIK_Q);
        break;
    case 3:
        SetKeys(DIK_J, DIK_L, DIK_I, DIK_K, DIK_O, DIK_Y, DIK_H);
        break;
    }
}

int CPumPlayer::CheckBHit(float fofsx, float fofsy, DWORD *pSrf)
{
    int     x,
    y,
    tmpXpos,
    tmpYpos;
    DWORD   *gptr = m_Gfx[ m_Frame ];

    RoundToInt(&tmpXpos, m_fXpos + fofsx);
    RoundToInt(&tmpYpos, m_fYpos + fofsy);


    for (y = 0; y < 4; y++) {
        for (x = 0; x < 5; x++) {
            if (*gptr) {
                if ((tmpYpos - 2 + y) < 0) return(1);
                if ((tmpYpos - 2 + y) >= ScreenHeight)  return(1);
                if ((tmpXpos - 1 + x) < 0) return(1);
                if ((tmpXpos - 1 + x) >= ScreenWidth)  return(1);

                if (pSrf[(tmpYpos - 2 + y) * ScreenWidth + (tmpXpos - 1 + x)] > 0)
                    return(1);
            }

            gptr++;
        }
    }

    return(0);
}

void CPumPlayer::Draw(DWORD *pSrf)
{
    int     x,
    y;
    DWORD   *gptr = m_Gfx[ m_Frame ];

    if (m_Alive == FALSE) return;

    for (y = 0; y < 4; y++) {
        for (x = 0; x < 5; x++) {
            if (*gptr)
                if ((m_iXpos + x - 2) >= 0 && (m_iXpos + x - 2) < ScreenWidth && (m_iYpos + y - 1) >= 0 && (m_iYpos + y - 1) < ScreenHeight)
                    pSrf[(m_iYpos - 2 + y) * ScreenWidth + (m_iXpos - 1 + x)] = *gptr;

            gptr++;
        }
    }

    //sprintf(tmp, "%d ", m_Health);
    //LilString( m_iXpos, m_iYpos-12, tmp, 0x00ffffff, pSrf );
    if (m_WeaponShow > 0.0f)
        LilStringCentered(m_iXpos, m_iYpos - 12, m_SP->Weapon->m_Weapons[ m_CurrentWpn ].m_Name, 0x00ffffff, pSrf);

    RoundToInt(&x, cosinus[ m_iAngle ] * 5.0f + (float)m_iXpos + 1);
    RoundToInt(&y, sinus[ m_iAngle ] * 5.0f + (float)m_iYpos - 1);

    if (x >= 0 && x < ScreenWidth && y >= 0 && y < ScreenHeight)
        pSrf[ y * ScreenWidth + x ] = (DWORD)((192 + (rand() & 63)) << 8);
}

int CPumPlayer::HitB(CPumBullet *d)
{
    float   fd;
    int     i, id,
    x, y;

    if (m_Alive == FALSE) return(0);

    x = d->m_iXpos - m_iXpos + 2;
    y = d->m_iYpos - m_iYpos + 1;

    if (x > 4) return(0);
    if (x < 0) return(0);
    if (y > 3) return(0);
    if (y < 0) return(0);

    i = y * 5 + x;

    id = RandInt(d->m_Damage / 2, d->m_Damage);
    fd = (float)id / (float)d->m_Damage;

    if (i == 2) { // head
        if (rand()&3)
            return(0);

        SpawnParticles(_PRT_BLOOD, (float)d->m_iXpos, (float)d->m_iYpos, 192, 16, 0, 2.5*TimeFactor, 0, 48, 0, 0, m_SP);
        id += 10;

        Damage(id, &d->m_Vec);

        m_AccuracyMod += RandFloat(0.1f, 1.0f);
        m_WalkSpeed *= RandFloat(0.98f, 1.0f);
        m_JumpPower *= RandFloat(0.98f, 1.0f);
    } else
        Damage(id, &d->m_Vec);

    return(1);
}

int CPumPlayer::HitP(CPumParticle *d)
{
    int     x, y;

    if (m_Alive == FALSE)
        return(0);

    if (!(d->m_Flags & PRT_LETHAL))
        return(0);

    x = d->m_iXpos - m_iXpos + 2;
    y = d->m_iYpos - m_iYpos + 1;

    if (x > 4) return(0);
    if (x < 0) return(0);
    if (y > 3) return(0);
    if (y < 0) return(0);

    if (!(rand()&7))
        m_Health --;

    return(1);
}

inline void CPumPlayer::EmergencyExit(DWORD *pSrf)
{
    int d,
    r;

    if (m_Alive == FALSE) return;

    for (r = 1; r < 128; r++) {
        for (d = 0; d < 256; d += 16) {
            if (!CheckBHit(cosinus[d] *(float)r, sinus[d] *(float)r, pSrf)) {
                m_fXpos += cosinus[d] * (float)r;
                m_fYpos += sinus[d] * (float)r;
                m_Vec.Set(0.0f, 0.0f);
                return;
            }
        }
    }
}

void CPumPlayer::Damage(int d, CDieselVector2 *v)
{
    SpawnParticles(_PRT_BLOOD, m_fXpos, m_fYpos, 40.74366f *((float)atan2(v->y, v->x) + 3.14159f), 32, 0, -d*TimeFactor / 10, 0, d*2, 0, 0, m_SP);
    SpawnParticles(_PRT_BLOOD, m_fXpos, m_fYpos, 0, 256, 0, -d*TimeFactor / 10, 0, d, 0, 0, m_SP);
    m_Health -= d;

    m_AccuracyMod += RandFloat(0.0f, 0.1f);
    m_WalkSpeed *= RandFloat(0.99f, 1.0f);
    m_JumpPower *= RandFloat(0.99f, 1.0f);

    if (m_AccuracyMod > 4.0f) m_AccuracyMod = 4.0f;
    if (m_WalkSpeed < 0.5f * TimeFactor) m_WalkSpeed = 0.5f * TimeFactor;
    if (m_JumpPower < 0.9f * TimeFactor) m_JumpPower = 0.9f * TimeFactor;
}
