#include "Creature.h"
#include "externals.h"


void Creature::draw(PicsContainer& pics){

    const COLOR c = ((gaveBirth)? COLOR(0.5,0.5, 1.f, 1.f - deathProgress) : COLOR(1.f, 1.f, 1.f, 1.f - deathProgress)) ;
    const COLOR procreationColor = (controled)? COLOR(0, 0, 1, 0.8) : COLOR(0,0,0.6f, 0.5f);

    if ((controled) && (!dead))
    {
        float fScale = radius / kfMaxRadius + 0.5f * pulsationProgress;
        pics.draw(mask, pos.v[0], pos.v[1], frame, true, fScale, fScale, 0);
    }


    pics.draw(race, pos.v[0], pos.v[1], (dead) ? 2 : frame, true, radius / kfMaxRadius, radius / kfMaxRadius, 0, c, c);

    if (dead)
    {
        return;
    }

    unsigned trace = 0;
    switch (race){
        case 3: trace = 0; break;
        case 2: trace = 1; break;
        case 5: trace = 2; break;
        case 6: trace = 3; break;
    }


    drawStatusBar(pics, pos.v[0]-15, pos.v[1]-22, hp, MaxHps[trace], COLOR(0.6f, 0.f, 0.f, 0.5f));
    drawStatusBar(pics, pos.v[0]-15, pos.v[1]-30, 
                  maxProcreationCount - procreationCount, maxProcreationCount, procreationColor);

    pics.draw(19, pos.v[0] - 15, pos.v[1] - 30, (isFemale)? 1 : 0);

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

void Creature::AI(float fDeltaTime, int iMaxAreaX, int iMaxAreaY, const LevelMap& map)
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
        if (movetics < 50)
        {
            movetics += fDeltaTime * 59.f;

            Vector3D posOld = pos;
            pos = pos + Vector3D(dir.v[0] * speed, dir.v[1] * speed, 0);

            if (!((pos.v[0] > radius) && (pos.v[0] < iMaxAreaX - radius)
              && (pos.v[1] > radius)&&(pos.v[1] < iMaxAreaY - radius)))
            {
                pos = posOld;
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

void Creature::fight(Creature& other)
{
    other.hp -= (rand() % (attack / 2)) + attack / 2;
    hp -= rand() % other.attack / 4;
}

void Creature::terraform(LevelMap& map)
{
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
