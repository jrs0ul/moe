#ifndef POPULATION_H
#define POPULATION_H

#include "Creature.h"
#include "LevelMap.h"

class Population{

    DArray<Creature> creatures;

public:

    Creature * get(unsigned index);
    unsigned long count(){return creatures.count();}
    void destroy(){creatures.destroy();}

    void nextActive(unsigned &Active, unsigned index);
    void makeChild(Creature* parent);
    void draw(PicsContainer& pics);
    void groundEffect(unsigned i, LevelMap& map);
    int  countByRace(unsigned race);
    void create(unsigned race1, unsigned race2);
};


#endif //POPULATION_H
