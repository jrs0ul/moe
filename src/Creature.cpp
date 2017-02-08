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
    iceBonus = 5;
    groundBonus = -2;
    waterBonus = -1;
    grassBonus = 0;
    fireBonus = -10;
    race = 2;
    mask = 14;
    hp = 85; 
}
//-----------------------------
void Creature::makeGoat(){
    iceBonus = -3;
    groundBonus = 2;
    waterBonus = -3;
    grassBonus = 5;
    fireBonus = -10;
    race = 3;
    mask = 15;
    hp = 65;
}
//-----------------------------
void Creature::makeSnake(){
    iceBonus = -4;
    groundBonus = 5;
    waterBonus = -2;
    grassBonus = 2;
    fireBonus = -10;
    race = 6;
    mask = 17;
    hp = 75;
}
//-----------------------------
void Creature::makeShark(){
    iceBonus = -3;
    groundBonus = -2;
    waterBonus = 5;
    grassBonus = 1;
    fireBonus = -10;
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

//---------------------------------------
void Creature::AI(){


    if (radius < 16){
        radius+=0.09f;
    }

    if (!controled){
        movetics++;
        int speed = 1;

        Vector3D posOld = pos;
        pos = pos + Vector3D(dir.v[0]*speed,
                         dir.v[1]*speed, 0);
        if (!((pos.v[0] > radius)&&(pos.v[0] < 640 - radius)&&(pos.v[1] > radius)&&(pos.v[1] < 480 - radius)))
            pos = posOld;

        if (movetics > 50){
            movetics = 0;
            int d = rand()%8;
            switch(d){
                case 0: dir = Vector3D(1, 0, 0); break;
                case 1: dir = Vector3D(-1, 0, 0); break;
                case 2: dir = Vector3D(0,-1,0); break;
                case 3: dir = Vector3D(0, 1, 0); break;
                case 4: dir = Vector3D(1, 1, 0); break;
                case 5: dir = Vector3D(-1, -1, 0); break;
                case 6: dir = Vector3D(1, -1, 0); break;
                case 7: dir = Vector3D(-1, 1, 0); break;
            }
        }
    }
                
}
