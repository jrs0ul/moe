#ifndef METEOR_H
#define METEOR_H

#include "Vectors.h"
#include "TextureLoader.h"
#include "audio/SoundSystem.h"

struct Cloud{
    Vector3D pos;
    COLOR c;
    float size;
    int tics;

    Cloud(){
        size = 1.0f;
        tics = 0;
    }
};


class Meteor
{
public:
   
    Meteor()
    {
        fireBallTics = 0;
        fireballPos = Vector3D(600,10,0);
        fireballDir = Vector3D(-1, 0.7,0);
        fireballFrame = 0;
        fireballAnimTics = 0;
    }

    void Render(PicsContainer& pics);
    void AnimateFireBall();
    void AnimateTrail();

    bool Update(SoundSystem& ss);

    void Destroy();

    int           fireBallTics;
    Vector3D      fireballPos;
    Vector3D      fireballDir;
    int           fireballFrame;
    int           fireballAnimTics;
    DArray<Cloud> trail;


};


#endif //METEOR_H
