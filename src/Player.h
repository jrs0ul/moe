#ifndef _PLAYER_H
#define _PLAYER_H

#include "Vectors.h"

struct Player{

    unsigned selectedRace;
    bool raceSelected;
    int raceIndex;

    unsigned activeCreature;
    unsigned Id;
    
    Vector3D    viewPortSize;
    Vector3D    viewPortPos;
    Vector3D    mapOffset;
    
    Player(){
        Id = 0;
        selectedRace = 0;
        raceSelected = false;
        raceIndex = 0;
        activeCreature = 0;
    }

};

#endif //_PLAYER_H
