#include "Creature.h"
#include "Utils.h"
#include "externals.h"


void Creature::draw(PicsContainer& pics, float OffsetX, float OffsetY, float ViewPortWidht, float ViewPortHeight)
{

    if (pos.v[0] - radius > -OffsetX + ViewPortWidht)
    {
        return;
    }

    const COLOR c = ((gaveBirth)? COLOR(0.5,0.5, 1.f, 1.f - deathProgress) : COLOR(1.f, 1.f, 1.f, 1.f - deathProgress)) ;
    const COLOR procreationColor = (controled)? COLOR(0, 0, 1, 0.8) : COLOR(0,0,0.6f, 0.5f);

    if ((controled) && (!dead))
    {
        float fScale = radius / kfMaxRadius + 0.5f * pulsationProgress;
        pics.draw(14, pos.v[0] + OffsetX, pos.v[1] + OffsetY, 0, true, fScale, fScale, 0);
    }


    pics.draw(race,
              pos.v[0] + OffsetX,
              pos.v[1] + OffsetY,
              (dead) ? 2 : frame, true, radius / kfMaxRadius, radius / kfMaxRadius, 0, c, c);

    if (dead)
    {
        return;
    }

    if (isWarrior)
    {
        pics.draw(21,
                  pos.v[0] + OffsetX,
                  pos.v[1] + OffsetY,
                  0, true, radius / kfMaxRadius, radius / kfMaxRadius, 0, c, c);
    }

    if (isBuilder)
    {
        pics.draw(22,
                  pos.v[0] + OffsetX,
                  pos.v[1] + OffsetY,
                  0, true, radius / kfMaxRadius, radius / kfMaxRadius, 0, c, c);
    }

    unsigned trace = 0;
    switch (race){
        case 3: trace = 0; break;
        case 2: trace = 1; break;
        case 5: trace = 2; break;
        case 6: trace = 3; break;
    }


    drawStatusBar(pics, pos.v[0]-15 + OffsetX, pos.v[1]-22 + OffsetY, hp, MaxHps[trace], COLOR(0.6f, 0.f, 0.f, 0.5f));
    drawStatusBar(pics, pos.v[0]-15 + OffsetX, pos.v[1]-30 + OffsetY, 
                  maxProcreationCount - procreationCount, maxProcreationCount, procreationColor);

    pics.draw(19,
              pos.v[0] - 15 + OffsetX,
              pos.v[1] - 30 + OffsetY,
              (isFemale)? 1 : 0);

}
//-----------------------------

void  Creature::makePenguin(){
    iTerrainBonuses[ET_ICE] = 5;
    iTerrainBonuses[ET_GROUND] = -2;
    iTerrainBonuses[ET_WATER] = 1;
    iTerrainBonuses[ET_GRASS] = 0;
    iTerrainBonuses[ET_LAVA] = -10;

    race = 2;
    mask = 14;
    hp = 85;
    attack = 10;
    maxProcreationCount = 3;
}
//-----------------------------
void Creature::makeGoat()
{
    iTerrainBonuses[ET_ICE] = -2;
    iTerrainBonuses[ET_GROUND] = 2;
    iTerrainBonuses[ET_WATER] = -3;
    iTerrainBonuses[ET_GRASS] = 5;
    iTerrainBonuses[ET_LAVA] = -10;

    race = 3;
    mask = 15;
    hp = 65;
    attack = 20;
    maxProcreationCount = 6;
}
//-----------------------------
void Creature::makeSnake(){

    iTerrainBonuses[ET_ICE] = -4;
    iTerrainBonuses[ET_GROUND] = 5;
    iTerrainBonuses[ET_WATER] = -2;
    iTerrainBonuses[ET_GRASS] = 2;
    iTerrainBonuses[ET_LAVA] = -10;

    race = 6;
    mask = 17;
    hp = 75;
    attack = 15;
    maxProcreationCount = 2;
}
//-----------------------------
void Creature::makeShark(){

    iTerrainBonuses[ET_ICE] = -1;
    iTerrainBonuses[ET_GROUND] = -2;
    iTerrainBonuses[ET_WATER] = 5;
    iTerrainBonuses[ET_GRASS] = -1;
    iTerrainBonuses[ET_LAVA] = -10;

    race = 5;
    mask = 16;
    hp = 55;
    attack = 25;
    maxProcreationCount = 4;
}

bool Creature::canProcreateWith(Creature* other) const
{

    return (((isFemale == false) && (procreationCount < maxProcreationCount) &&
            (other->isFemale == true) && (!other->gaveBirth)) ||
            ((isFemale == true) && (!gaveBirth) &&
            (other->isFemale == false)&&(other->procreationCount < other->maxProcreationCount)));

}

void Creature::drawStatusBar(PicsContainer& pics, 
                             float x, float y, float current, float max, const COLOR& c)
{
    pics.draw(-1, x, y, 0, false, 30, 8, 0, COLOR(0.5,0.5,0.5,0.5));
    float prcnt = current / (max / 100.0f);
    pics.draw(-1, x, y, 0, false, 0.30 * prcnt, 8, 0, c, c);


}

//-----------------------------
void Creature::animate(){
    animtics++;
    if (animtics > 15){
        frame++;
        animtics = 0;
        if (frame > 1)
            frame = 0;
    }
}

void Creature::fight(Creature& other)
{
    playAttachedSound(1);
    int damage = Random(0, attack / 2) + attack / 2;
    other.hp -= damage;
    int oponentDamage = Random(0, other.attack / 2);
    hp -= oponentDamage;

    printf("your damage: %d oponent's :%d\n", damage, oponentDamage);
    if (other.hp < 0)
    {
        printf("-------------\n");
    }
}

void Creature::terraform(LevelMap& map)
{
    if (!isBuilder)
    {
        return;
    }

    unsigned mapX = (pos.v[0]) / 32;
    unsigned mapY = (pos.v[1]) / 32;


    int iTerrain = map.getTerrainType(mapX, mapY);

    if (iTerrain != -1)
    {

        int iVal = -10;
        int iIndex = -1;

        for (int i = 0; i < ET_COUNT; ++i)
        {
            if (iTerrainBonuses[i] > iVal)
            {
                iVal = iTerrainBonuses[i];
                iIndex = i;
            }
        }
        

        if (iIndex > -1)
        {
            map.setTerrainType(mapX, mapY, (TerrainTypes)iIndex);
        }
    }
}

void Creature::Move(float fSpeed, int iMaxAreaX, int iMaxAreaY)
{
    Vector3D vNewPos = pos + (Vector3D(dir.v[0] * fSpeed, dir.v[1] * fSpeed, 0));
            
    if ((vNewPos.v[0] + radius < iMaxAreaX) && (vNewPos.v[0] > radius)
                    && (vNewPos.v[1] + radius < iMaxAreaY) && (vNewPos.v[1] > radius))
    {
        pos = vNewPos;
    }

}

void Creature::AI(float fDeltaTime, 
                  int iMaxAreaX, int iMaxAreaY,
                  const LevelMap& map,
                  DArray<Creature>& creatures)
{

    if (dead)
    {
        if (deathProgress < 1.f)
        {
            deathProgress += fDeltaTime * 0.294f;
        }

        return;
    }

    if (procreating == true)
    {
        procreationProgress += fDeltaTime * 0.5f;
        if (procreationProgress > 1.f)
        {
            procreationProgress = 0.f;
            procreating = false;
            if (isFemale)
            {
                givesBirth = true;
            }
        }
        return;
    }

    if (controled)
    {
        pulsationProgress += (fDeltaTime * 0.5882f * pulseMultiplier);

        if (pulsationProgress > 1.f)
        {
            pulseMultiplier = -1.f;
        }
        else if (pulsationProgress < -0.125f)
        {
            pulseMultiplier = 1.f;
        }
    }

    const int speed = 1;

    if (radius < kfMaxRadius)
    {
        radius += kfGrowthIncrease;
    }

    if (controled == false)
    {

        if (isWarrior)
        {
            haveDir = false;
            /*if (haveDir == false)
            {*/
                float shortestDistance = 999.f;

                for (unsigned i = 0; i < creatures.count(); ++i)
                {
                    Creature* c = &creatures[i];

                    if (c->race != race && !c->dead)
                    {
                        c->pos.v[2] = 0;
                        pos.v[2] = 0;
                        Vector3D d = c->pos - pos;
                        float len = d.length();

                        if (len < shortestDistance)
                        {
                            shortestDistance = len;
                            dir = d;
                        }

                        haveDir = true;
                        
                    }

                }

                if (haveDir)
                {
                    dir.normalize();
                }

            //}
        }

        if ((movetics < 50) || (isWarrior && haveDir)) 
        {
            movetics += fDeltaTime * 59.f;

            Vector3D posOld = pos;
            pos = pos + Vector3D(dir.v[0] * speed, dir.v[1] * speed, 0);

            if (!((pos.v[0] > radius) && (pos.v[0] < iMaxAreaX - radius)
              && (pos.v[1] > radius)&&(pos.v[1] < iMaxAreaY - radius)))
            {
                pos = posOld;
            }

            //check if collides with target
            if (isWarrior)
            {
                for (unsigned i = 0; i < creatures.count(); ++i)
                {
                    Creature* c = &creatures[i];

                    if (c->race != race && !c->dead)
                    {
                        if (CirclesColide(pos.v[0], pos.v[1], radius, c->pos.v[0], c->pos.v[1], c->radius))
                        {
                            attackProgress += fDeltaTime * 1.3f;

                            if (attackProgress > 1.f)
                            {
                                fight(*c);
                                attackProgress = 0.f;
                            }
                        }
                        
                    }

                }
            }

        }
        else
        {
            int d = rand()%8;
            const Vector3D vArray[8] = {{ 1, 0, 0}, 
                                        {-1, 0, 0},
                                        { 0,-1, 0},
                                        { 0, 1, 0},
                                        { 1, 1, 0},
                                        {-1,-1, 0},
                                        { 1,-1, 0},
                                        {-1, 1, 0}};
            dir = vArray[d];

            unsigned mapX = (pos.v[0] +  dir.v[0] * speed * 25) / 32;
            unsigned mapY = (pos.v[1] +  dir.v[1] * speed * 25) / 32;


            int iTerrain = map.getTerrainType(mapX, mapY);

            if (iTerrain < 0)
            {
                return;
            }
            
            if (iTerrainBonuses[iTerrain] >= 0)
            {
                movetics = 0;
            }
            
        }
    }
}

void Creature::attachBuffer(SoundSystem& ss, unsigned int index, unsigned int place){
    
    if (alIsSource(soundSources[place]))
        alDeleteSources(1, &soundSources[place]);
    alGenSources(1, &soundSources[place]);
    alSourcei(soundSources[place], AL_BUFFER, ss.getBuffer(index));
    
    //ALenum r=0;
    //r=alGetError();
    //if ( r != AL_NO_ERROR){
    //    printf("Error: %x while attaching\n",r);
    //}
   // else
   //     puts("attached");
}


void Creature::freeSoundSource(unsigned int place){
    if (alIsSource(soundSources[place])){
        alSourceStop(soundSources[place]);
        alDeleteSources(1, &soundSources[place]);
    }
}
//---------------------------------
void Creature::playAttachedSound(unsigned int place, float volume){
    if (alIsSource(soundSources[place])){
        /*alSource3f(soundSources[place], AL_POSITION, pos.x(), pos.z(), pos.y());
        alSource3f(soundSources[place], AL_DIRECTION, direction.x(), direction.z(), direction.y());
        alSource3f(soundSources[place], AL_VELOCITY, direction.x() * speed, direction.z(), direction.y() * speed);
        alSourcei (soundSources[place], AL_SOURCE_RELATIVE, AL_FALSE);
        alSourcef(soundSources[place], AL_CONE_OUTER_ANGLE, 360.0f);
        alSourcef(soundSources[place], AL_CONE_INNER_ANGLE, 360.0f);
        alSourcef(soundSources[place], AL_MAX_DISTANCE, 32.0f * ((place == 2) ? 10 : 5));
        alSourcef(soundSources[place], AL_REFERENCE_DISTANCE, 32.0f);
        alSourcef(soundSources[place], AL_GAIN, ((place == 2) ? 1.0 : 0.6f)*volume);
        alSourcef(soundSources[place], AL_PITCH, 1.0f);*/
        alSourcePlay(soundSources[place]);
        //ALenum r = 0;
        //r = alGetError();
        /*if ( r != AL_NO_ERROR){
            printf("Error: %x while playing\n",r);
        }*/
    }
   // else 
   //     puts("bad source");

}



