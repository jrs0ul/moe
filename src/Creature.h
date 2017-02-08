#ifndef _CREATURE_H
#define _CREATURE_H

#include "Vectors.h"
#include "TextureLoader.h"


class Creature{
public:
    Vector3D pos;
    Vector3D dir;
    float radius;
    int hp;
    unsigned race;
    unsigned mask;
    unsigned frame;
    unsigned animtics;

    unsigned damageTics;

    int movetics;
    bool controled;

    int iceBonus;
    int groundBonus;
    int waterBonus;
    int grassBonus;
    int fireBonus;

    bool dead;
    bool gaveBirth;

    bool haveDir;


    Creature(){
        movetics = 0;
        hp = 50;
        radius = 16;
        frame = 0;
        animtics = 0;
        mask = 0;
        controled = false;
        iceBonus = 0;
        groundBonus = 0;
        waterBonus = 0;
        grassBonus = 0;
        fireBonus = 0;
        damageTics = 0;
        dead = false;
        gaveBirth = false;

        haveDir = false;
        dir = Vector3D(0,0,0);
    }

    void draw(PicsContainer& pics);
    void AI();
    void animate();
    void makePenguin();
    void makeGoat();
    void makeSnake();
    void makeShark();


};


#endif //CREATURE_H
