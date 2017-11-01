#include "PowerUp.h"

void PowerUp::Render(PicsContainer& pics, unsigned pic)
{
    float fScale = ((bGrow) ? fAnimationProgress : 1 - fAnimationProgress) * 0.25f;

    COLOR c = COLOR(1.f, 1.f, 1.f, 1 - lifeTime);

    pics.draw(pic, pos.v[0], pos.v[1], type, true, 1.f + fScale, 1.f + fScale, 0.f, c, c);
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

void PowerUpArray::Render(PicsContainer& pics, unsigned uImageIndex)
{

    for (unsigned i = 0; i < m_PowerUps.count(); ++i)
    {
        if (!m_PowerUps[i].pickedUp)
        {
            m_PowerUps[i].Render(pics, 20);
        }
    }

}

