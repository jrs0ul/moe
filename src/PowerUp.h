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


    void Render(PicsContainer& pics, unsigned pic);

    Vector3D pos;
    float    radius;
    int      type;
    


};

#endif // POWERUP_H
