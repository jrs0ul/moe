#ifndef POWERUP_H
#define POWERUP_H

#include "TextureLoader.h"
#include "Vectors.h"

enum PowerUpTypes
{
    PT_FEMALE = 0,
    PT_WARRIOR,
    PT_BUILDER,
    PT_TIMEUP,
    PT_TIMEDOWN,
    PT_FERTILITY,
    PT_COUNT
};


class PowerUp
{
public:


    PowerUp()
    : pos(0, 0, 0)
    , radius(1.f)
    , lifeTime(0.f)
    , type(0)
    , pickedUp(false)
    {}

    void Render(PicsContainer& pics, unsigned pic);
    void Update(float fDeltaTime);

    Vector3D pos;
    float    radius;
    float    lifeTime;
    int      type;
    bool     pickedUp;
private:
    bool    bGrow;
    float   fAnimationProgress;
    


};


class PowerUpArray
{
public:

    void Update(float fDeltaTime);
    void Render(PicsContainer& pics, unsigned uImageIndex);
    bool FindNearestPowerUp(int type, const Vector3D& pos, Vector3D& direction);

    DArray<PowerUp> m_PowerUps;
};

#endif // POWERUP_H
