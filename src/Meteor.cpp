#include "Meteor.h"

void Meteor::Render(PicsContainer& pics)
{
    for (unsigned i = 0; i < trail.count(); i++)
    {
        pics.draw(12, trail[i].pos.v[0], trail[i].pos.v[1], 0, true,
                      trail[i].size, trail[i].size, 0, trail[i].c, trail[i].c);
    }

    pics.draw(4, 
              fireballPos.v[0],
              fireballPos.v[1],
              fireballFrame,
              true,
              3.7f - fireBallTics/80.0f,
              3.7f - fireBallTics/80.0f);

}

void Meteor::AnimateFireBall()
{
    fireballPos = fireballPos +
                      Vector3D(fireballDir.v[0]*2,fireballDir.v[1]*2,0);
    fireballAnimTics++;
    if (fireballAnimTics > 10){

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

bool Meteor::Update(SoundSystem& ss)
{
    fireBallTics++;
    if (fireBallTics == 150){

        if (!ss.isPlaying(0)){
            ss.playsound(0);
            trail.destroy();
        }
        return true;
    }

    return false;
 
}

void Meteor::Destroy()
{
    fireBallTics = 0;
    fireballPos = Vector3D(600,10,0);
    fireballDir = Vector3D(-1, 0.7,0);
    trail.destroy();
}
