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
    : fProgress(0.f)
    , soundSourceCount(0)
    {
        fireBallOrigin = Vector3D(600,10,0);
        fireballDir = Vector3D(0.f, 0.f,0);
        fireballFrame = 0;
        fireballAnimTics = 0;
    }

    ~Meteor();

    void Render(PicsContainer& pics, float OffsetX = 0.f, float OffsetY = 0.f);
    void AnimateFireBall();
    void AnimateTrail();
    void AttachSoundSources(SoundSystem& ss, unsigned index);
    void SetDestination(float x, float y);

    bool Update(float fDeltaTime);


    float         fProgress;
    int           fireballFrame;
    int           fireballAnimTics;
    DArray<Cloud> trail;
private:

    Vector3D fireBallOrigin;
    Vector3D fireBallDestination;
    Vector3D fireballPos;
    Vector3D fireballDir;
    unsigned soundSources[3];
    unsigned soundSourceCount;


};


#endif //METEOR_H
