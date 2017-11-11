#ifndef _PLAYER_H
#define _PLAYER_H

struct Player{

    unsigned selectedRace;
    bool raceSelected;
    int raceIndex;

    unsigned activeCreature;
    unsigned Id;
    
    Player(){
        Id = 0;
        selectedRace = 0;
        raceSelected = false;
        raceIndex = 0;
        activeCreature = 0;
    }

};

#endif //_PLAYER_H
