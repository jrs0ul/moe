#include "Creature.h"
#include "externals.h"


void Creature::draw(PicsContainer& pics){
    if (!dead){

        COLOR c = ((gaveBirth)? COLOR(0.5,0.5,1) : COLOR(1,1,1)) ;

        if ((controled)&&(rand()%2))
            pics.draw(mask, pos.v[0], pos.v[1], frame, true, 
                  radius/16.0f + 0.15f, radius/16.0f + 0.15f, 0);


        pics.draw(race, pos.v[0], pos.v[1], frame, true, 
                  radius/16.0f, radius/16.0f, 0,
                  c, c);

        unsigned trace = 0;
        switch (race){
            case 3: trace = 0; break;
            case 2: trace = 1; break;
            case 5: trace = 2; break;
            case 6: trace = 3; break;
        }

        pics.draw(-1, pos.v[0]-15, pos.v[1]-22, 0, false, 30, 8, 0, COLOR(0.5,0.5,0.5,0.5));
       float prcnt = hp/(MaxHps[trace]/100.0f);
       pics.draw(-1, pos.v[0]-15, pos.v[1]-22, 0, false, 0.30*prcnt, 8, 0, COLOR(0.6,0,0,0.5), COLOR(0.6,0,0,0.5));
    }

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

void Creature::AI(int iMaxAreaX, int iMaxAreaY, const LevelMap& map)
{
    
    const int speed = 1;

    if (radius < kfMaxRadius)
    {
        radius += kfGrowthIncrease;
    }

    if (controled == false)
    {
        if (movetics < 50)
        {
            movetics++;

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
