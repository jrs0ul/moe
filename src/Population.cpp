#include "Population.h"
#include "externals.h"
#include "Utils.h"


void Population::Update(float fDeltaTime, LevelMap& Mapas, 
                        int iScreenWidth, int iScreenHeight, 
                        unsigned& Player1ActiveCreature, unsigned& Player2ActiveCreature,
                        bool startImpact, bool showWinner)
{
    for (unsigned i = 0; i < count(); ++i)
    {
        Creature * c = get(i);

        if (!startImpact)
        {
            c->AI(fDeltaTime, iScreenWidth, iScreenHeight, Mapas);
        }

        c->animate();

        if (c->givesBirth == true)
        {
            makeChild(c);
            c->givesBirth = false;
        }

        if (!showWinner)
        {
            groundEffect(i, Mapas);

            if (c->hp <= 0){
                c->dead = true;
                
                if (i == Player1ActiveCreature)
                {
                    nextActive(Player1ActiveCreature, i);
                }

                if (i == Player2ActiveCreature)
                {
                    nextActive(Player2ActiveCreature, i);
                }
            }
            //----------------------
        }
    }

}

Creature * Population::get(unsigned index)
{
    if (index < creatures.count())
        return &creatures[index];

    return 0;
}

void Population::nextActive(unsigned &Active, unsigned index)
{
    
    DArray<unsigned> tmp;

    unsigned irace = creatures[index].race;
    for (unsigned a = 0; a < creatures.count(); a++)
    {
        if (((creatures[a].race == irace) && (!creatures[a].dead)) || (a == index))
        {
            tmp.add(a);
        }
    }

    creatures[Active].controled = false;
    creatures[Active].pulsationProgress = 0.f;
    creatures[Active].pulseMultiplier = 1.f;

    for (unsigned a = 0; a < tmp.count(); a++)
    {
        if (tmp[a] == Active)
        {
            if (a + 1 < tmp.count())
            {
                Active = tmp[a+1];
            }
            else
            {
                Active = tmp[0];
            }
            break;
        }
    }

    creatures[Active].controled = true;
    creatures[Active].pulsationProgress = 0.f;
    creatures[Active].pulseMultiplier = 1.f;
    tmp.destroy();
}
//------------------------
void Population::makeChild(Creature * parent)
{

    Creature child;
    parent->gaveBirth = true;
    child.pos = parent->pos;
    child.race = parent->race;
    child.mask = parent->mask;
    child.radius = 8;

    unsigned trace = 0;
    switch (parent->race){
        case 3: trace = 0; break;
        case 2: trace = 1; break;
        case 5: trace = 2; break;
        case 6: trace = 3; break;
    }

    for (int i = 0; i < ET_COUNT; ++i)
    {
        child.iTerrainBonuses[i] = parent->iTerrainBonuses[i];
    }

    child.hp = MaxHps[trace]; 

    creatures.add(child);

}
//---------------------------
void Population::draw(PicsContainer& pics){

    DArray<Creature*> controledOnes;

    for (unsigned i = 0; i < creatures.count(); i++)
    {
        if (creatures[i].controled)
        {
            controledOnes.add(&creatures[i]);
        }
        else
        {
            creatures[i].draw(pics);
        }
    }

    if (controledOnes.count())
    {
        for (unsigned i = 0; i < controledOnes.count(); ++i)
        {
            controledOnes[i]->draw(pics);
        }
    }

    controledOnes.destroy();

}


void Population::interact(unsigned interactor)
{

    Creature* c = get(interactor);

    for (unsigned i = 0; i < count(); ++i)
    {
        Creature * other = get(i);

        bool bColides = CirclesColide(c->pos.v[0],
                            c->pos.v[1],
                            c->radius,
                            other->pos.v[0],
                            other->pos.v[1],
                            other->radius);

        if ((!bColides) || (interactor == i) || (other->dead))
        {
            continue;
        }


        const bool bCanProcreate = c->canProcreateWith(other);


        if ((bCanProcreate) && (other->radius > 15.5f) && (c->radius > 15.5f)
            &&(other->race == c->race) 
            &&(other->procreating == false) && (c->procreating == false))
        {
            if (c->isFemale == false)
            {
                c->procreationCount++;
            }
            c->procreating = true;
            other->procreating = true;
            other->procreationProgress = 0.f;
            c->procreationProgress = 0.f;
        }
        else if (other->race != c->race)
        {
            c->fight(*other);
        }
    }

}

//-------------------------
void Population::groundEffect(unsigned i, LevelMap& map){

    creatures[i].damageTics++;

    if (creatures[i].damageTics > 10){
        creatures[i].damageTics = 0;
        int x = creatures[i].pos.v[0] / 32;
        int y = (creatures[i].pos.v[1]+15) / 32;

        if ((x < 0)||(x >= (int)MaxMapWidth))
            return;
        if ((y < 0)||(y >= (int)MaxMapHeight))
            return;

        int TerrainType = map.getTerrainType(x, y);

        creatures[i].hp += creatures[i].iTerrainBonuses[TerrainType];

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
