#ifndef POPULATION_H
#define POPULATION_H

#include "Creature.h"
#include "LevelMap.h"

class Population{


public:

    void Update(float fDeltaTime, LevelMap& Mapas,
                int iScreenWidth, int iScreenHeight,
                unsigned& Player1ActiveCreature, unsigned& Player2ActiveCreature,
                bool startImpact, bool showWinner);
    Creature * get(unsigned index);
    unsigned long count(){return creatures.count();}
    void destroy(){creatures.destroy();}

    void nextActive(unsigned &Active, unsigned index);
    void draw(PicsContainer& pics);
    void interact(unsigned interactor);
    void groundEffect(unsigned i, LevelMap& map);
    int  countByRace(unsigned race);
    void create(unsigned race1, unsigned race2);

private:

    void makeChild(Creature* parent);

private:
    DArray<Creature> creatures;

};


#endif //POPULATION_H
