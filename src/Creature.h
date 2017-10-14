#ifndef _CREATURE_H
#define _CREATURE_H

#include "Vectors.h"
#include "TextureLoader.h"
#include "LevelMap.h"


const float kfMaxRadius = 16.f;
const float kfGrowthIncrease = 0.09f;

class Creature{
public:
    Creature(){
        movetics = 0;
        hp = 50;
        radius = 16;
        frame = 0;
        animtics = 0;
        mask = 0;
        controled = false;
        damageTics = 0;
        dead = false;
        gaveBirth = false;

        haveDir = false;
        dir = Vector3D(0.f, 0.f, 0.f);

        for (int i = 0; i < ET_COUNT; ++i)
        {
            iTerrainBonuses[i] = 0;
        }
    }

    void draw(PicsContainer& pics);
    void AI(int iMaxAreaX, int iMaxAreaY, const LevelMap& map);
    void animate();
    void makePenguin();
    void makeGoat();
    void makeSnake();
    void makeShark();


    Vector3D pos;
    Vector3D dir;
    float radius;
    int hp;
   
    int movetics;

    int iTerrainBonuses[ET_COUNT];

    /*int iceBonus;
    int groundBonus;
    int waterBonus;
    int grassBonus;
    int fireBonus;*/

    unsigned race;
    unsigned mask;
    unsigned frame;
    unsigned animtics;
    unsigned damageTics;


    bool dead;
    bool gaveBirth;

    bool haveDir;
    bool controled;
};


#endif //CREATURE_H
