#include "Population.h"
#include "externals.h"

Creature * Population::get(unsigned index){
    if (index < creatures.count())
        return &creatures[index];

    return 0;
}
//-------------------
void Population::nextActive(unsigned &Active, unsigned index){
    DArray<unsigned> tmp;
    unsigned irace = creatures[index].race;
    for (unsigned a = 0; a < creatures.count(); a++){
        if (((creatures[a].race == irace)&&(!creatures[a].dead))||(a == index))
            tmp.add(a);
    }
    for (unsigned a = 0; a < tmp.count(); a++){
        if (tmp[a] == Active){
            if (a + 1 < tmp.count())
                Active = tmp[a+1];
            else
                Active = tmp[0];
            break;
        }
    }
    creatures[Active].controled = true;
    tmp.destroy();
}
//------------------------
void Population::makeChild(Creature * parent){

    Creature child;
    parent->gaveBirth = true;
    child.pos = parent->pos;
    child.race = parent->race;
    child.dir = Vector3D(0,0,0);
    child.radius = 8;

    unsigned trace = 0;
    switch (parent->race){
        case 3: trace = 0; break;
        case 2: trace = 1; break;
        case 5: trace = 2; break;
        case 6: trace = 3; break;
    }
    child.groundBonus = parent->groundBonus;
    child.waterBonus = parent->waterBonus;
    child.grassBonus = parent->grassBonus;
    child.fireBonus = parent->fireBonus;
    child.iceBonus = parent->iceBonus;

    child.hp = MaxHps[trace]; 

    creatures.add(child);

}
//---------------------------
void Population::draw(PicsContainer& pics){
    for (unsigned i = 0; i < creatures.count(); i++){
        creatures[i].draw(pics);
    }

}
//-------------------------
void Population::groundEffect(unsigned i, LevelMap& map){
    creatures[i].damageTics++;
    if (creatures[i].damageTics > 10){
        creatures[i].damageTics = 0;
        int x = creatures[i].pos.v[0]/32;
        int y = (creatures[i].pos.v[1]+15)/32;

        if ((x < 0)||(x >= (int)MaxMapWidth))
            return;
        if ((y < 0)||(y >= (int)MaxMapHeight))
            return;

        int TerrainType = map.map[y][x].picture;
        if (map.map[y][x].picture > 2){
            if (map.map[y][x].picture == 6)
                TerrainType = 3;
            if (map.map[y][x].picture > 6)
                TerrainType = 4;

            if (map.map[y][x].picture < 6)
                TerrainType = 2;
        }

        switch(TerrainType){
            case 0: {
                creatures[i].hp+= creatures[i].groundBonus;
            }break;
            case 1: {
                creatures[i].hp+= creatures[i].grassBonus;
            }break;
            case 2: {
                creatures[i].hp+= creatures[i].waterBonus;
            }break;
            case 3: {
                creatures[i].hp+= creatures[i].iceBonus;
            }break;
            case 4: {
                creatures[i].hp+= creatures[i].fireBonus;
            }break;
        }

        unsigned race = creatures[i].race;
        unsigned trace = 0;
        switch (race){
            case 3: trace = 0; break;
            case 2: trace = 1; break;
            case 5: trace = 2; break;
            case 6: trace = 3; break;
        }

        if (creatures[i].hp > MaxHps[trace])
            creatures[i].hp = MaxHps[trace];
        
        if (creatures[i].hp < 0)
            creatures[i].hp = 0;

    }
}
//----------------------------
int Population::countByRace(unsigned race){
    int result = 0;
    for (unsigned i = 0; i < creatures.count(); i++){
        if ((creatures[i].race == race)&&(!creatures[i].dead)){
            result++;
        }
    }
    return result;
}
//-----------------------------
void Population::create(unsigned race1, unsigned race2){
    for (unsigned i = 0; i < 10; i++){
        Creature c;

        if (i > 4){ 
            c.pos = Vector3D(rand()%100+50, rand()%200+50, 0);
            switch(race1){
            case 0: c.makeGoat();break;
            case 1: c.makePenguin();break;
            case 2: c.makeShark();break;
            case 3: c.makeSnake();break;
            }

        }
        else{//ožis
            c.pos = Vector3D(rand()%100+500, rand()%200+50, 0);
            switch(race2){
            case 0:c.makeGoat();break;
            case 1:c.makePenguin();break;
            case 2:c.makeShark();break;
            case 3:c.makeSnake();break;
            }

        }
        c.dir = Vector3D(0, 0, 0);
        creatures.add(c);
    }

}
