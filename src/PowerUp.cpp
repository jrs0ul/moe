#include "PowerUp.h"

void PowerUp::Render(PicsContainer& pics, unsigned pic, float OffsetX, float OffsetY)
{
    float fScale = ((bGrow) ? fAnimationProgress : 1 - fAnimationProgress) * 0.25f;

    COLOR c = COLOR(1.f, 1.f, 1.f, 1 - lifeTime);

    pics.draw(pic, 
              pos.v[0] + OffsetX,
              pos.v[1] + OffsetY,
              type, true, 1.f + fScale, 1.f + fScale, 0.f, c, c);
}

void PowerUp::Update(float fDeltaTime)
{
    fAnimationProgress += fDeltaTime;

    if (fAnimationProgress > 1.f)
    {
        bGrow = !bGrow;
        fAnimationProgress = 0.f;
    }

    lifeTime += fDeltaTime * 0.1f;

    if (lifeTime > 1.f)
    {
        pickedUp = true;
    }
}

//---

void PowerUpArray::Update(float fDeltaTime)
{
    for (unsigned i = 0; i < m_PowerUps.count(); ++i)
    {
        if (!m_PowerUps[i].pickedUp)
        {
            m_PowerUps[i].Update(fDeltaTime);
        }
    }
}

void PowerUpArray::Render(PicsContainer& pics, unsigned uImageIndex, float OffsetX, float OffsetY)
{

    for (unsigned i = 0; i < m_PowerUps.count(); ++i)
    {
        if (!m_PowerUps[i].pickedUp)
        {
            m_PowerUps[i].Render(pics, 20, OffsetX, OffsetY);
        }
    }

}

bool PowerUpArray::FindNearestPowerUp(int type, const Vector3D& pos, Vector3D& direction)
{
    float shortestDistance = 10000.0f;
    Vector3D dir = Vector3D(1,0,0);

    bool found = false;
        
    for (unsigned i = 0; i < m_PowerUps.count(); i++)
    {
        PowerUp* pUp = &m_PowerUps[i];

        if (!pUp->pickedUp && pUp->type == type)
        {
            pUp->pos.v[2] = 0;
            Vector3D d = pUp->pos - pos;
            const float len = d.length();

            if (len < shortestDistance)
            {
                shortestDistance = len;
                dir = d;
            }

            found = true;
        }
    }

    direction = dir;
    return found;

}

