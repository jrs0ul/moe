#include "Meteor.h"
#include "Utils.h"

Meteor::~Meteor()
{
    trail.destroy();

    if (alIsSource(soundSources[0])){
        alSourceStop(soundSources[0]);
        alDeleteSources(1, &soundSources[0]);
    }

    printf("destructed\n");

}


void Meteor::Render(PicsContainer& pics, float OffsetX, float OffsetY)
{
    for (unsigned i = 0; i < trail.count(); i++)
    {
        pics.draw(12, 
                  trail[i].pos.v[0] + OffsetX,
                  trail[i].pos.v[1] + OffsetY,
                  0, true,
                  trail[i].size, trail[i].size, 0, trail[i].c, trail[i].c);
    }

    pics.draw(4, 
              fireballPos.v[0] + OffsetX,
              fireballPos.v[1] + OffsetY,
              fireballFrame,
              true,
              3.7f - 2.f * fProgress,
              3.7f - 2.f * fProgress);

}

void Meteor::AnimateFireBall()
{

    fireballPos = Lerp(fireBallOrigin, fireBallDestination, fProgress);

    fireballAnimTics++;

    if (fireballAnimTics > 10)
    {

        Cloud c;
        c.pos = fireballPos;
        c.c = COLOR(1,1,1,1);
        c.size = 1.5f;
        trail.add(c);

        fireballAnimTics = 0;
        fireballFrame++;
        if (fireballFrame > 2)
        {
            fireballFrame = 0;
        }
    }
}

void Meteor::AnimateTrail()
{
    for (unsigned i = 0; i < trail.count(); i++){
        trail[i].tics++;
        if (trail[i].tics > 4){
            trail[i].size -= 0.05f;
            trail[i].tics = 0;
            trail[i].c.c[3]-=0.025f;
        }
    }

}

void Meteor::AttachSoundSources(SoundSystem& ss, unsigned index)
{
    if (soundSourceCount > 2)
    {
        return;
    }

    ss.AttachBufferToSource(index, soundSources[soundSourceCount]);

    soundSourceCount++;
}

void Meteor::SetDestination(float x, float y)
{
    fireBallDestination = Vector3D(x, y, 0.f);
}

bool Meteor::Update(float fDeltaTime)
{
    fProgress += fDeltaTime * 0.5f;
    
    if (fProgress > 1.f){

        if (alIsSource(soundSources[0]))
        {
            alSourcePlay(soundSources[0]);
        }

        trail.destroy();
        return true;
    }

    return false;
 
}
