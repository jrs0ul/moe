#include "Population.h"
#include "externals.h"
#include "Utils.h"


void Population::Update(float fDeltaTime,
                        LevelMap& Mapas,
                        SoundSystem& ss,
                        PowerUpArray& PowerUps,
                        int& secondsUntilImpact,
                        int iScreenWidth, int iScreenHeight, 
                        unsigned& Player1ActiveCreature, unsigned& Player2ActiveCreature,
                        bool startImpact, bool showWinner)
{
    for (unsigned i = 0; i < count(); ++i)
    {
        Creature * c = get(i);

        if (!startImpact)
        {
            c->AI(fDeltaTime, Mapas, creatures);
        }

        c->animate();

        if (c->givesBirth == true)
        {
            makeChild(c);
            c->givesBirth = false;
        }


        if ((i == Player1ActiveCreature) || (i == Player2ActiveCreature))
        {
            for (unsigned j = 0; j < PowerUps.m_PowerUps.count(); ++j)
            {
                PowerUp* pUp = &PowerUps.m_PowerUps[j];

                if (!pUp->pickedUp)
                {
                    bool bColides = CirclesColide(c->pos.v[0],
                                                  c->pos.v[1],
                                                  c->radius,
                                                  pUp->pos.v[0],
                                                  pUp->pos.v[1],
                                                  pUp->radius);
                    if (bColides)
                    {
                        pUp->pickedUp = true;

                        switch (pUp->type)
                        {
                            case PT_FEMALE: SpawnAFemale(c->race); break;
                            case PT_WARRIOR: c->makeWarrior(); break;
                            case PT_BUILDER: c->makeBuilder(); break;
                            case PT_TIMEUP: secondsUntilImpact += 5; break;
                            case PT_TIMEDOWN: secondsUntilImpact -= 1 ; break;
                            case PT_FERTILITY: Fertilize(c->race); break;
                        }
                    }

                }
            }
        }

        if (!showWinner)
        {
            groundEffect(i, Mapas);

            if (c->hp <= 0)
            {
                if (!c->dead)
                {
                    c->playAttachedSound(0);
                    c->dead = true;
                }
                
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

void Population::destroy()
{
    for (unsigned i = 0; i < creatures.count(); ++i)
    {
        creatures[i].freeSoundSource(1);
        creatures[i].freeSoundSource(0);
    }

    creatures.destroy();
}

void Population::nextActive(unsigned &Active, unsigned index)
{
    
    DArray<unsigned> tmp;

    unsigned irace = creatures[index].race;
    for (unsigned a = 0; a < creatures.count(); a++)
    {
        const Creature* c = &creatures[a];

        if (((c->race == irace) && (!c->dead) && (!c->procreating) && (!c->givesBirth))
            || (a == index))
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
//---------------------------
void Population::draw(PicsContainer& pics, 
                      float OffsetX, float OffsetY,
                      float ViewPortWidht, float ViewPortHeight){

    DArray<Creature*> controledOnes;

    for (unsigned i = 0; i < creatures.count(); i++)
    {
        if (creatures[i].controled)
        {
            controledOnes.add(&creatures[i]);
        }
        else
        {
            creatures[i].draw(pics, OffsetX, OffsetY, ViewPortWidht, ViewPortHeight);
        }
    }

    if (controledOnes.count())
    {
        for (unsigned i = 0; i < controledOnes.count(); ++i)
        {
            controledOnes[i]->draw(pics, OffsetX, OffsetY, ViewPortWidht, ViewPortHeight);
        }
    }

    controledOnes.destroy();

}


void Population::interact(unsigned interactor, SoundSystem& ss)
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
        else if ((other->race != c->race) && (c->isWarrior))
        {
            c->fight(*other);
        }
    }

}

//-------------------------
void Population::groundEffect(unsigned i, LevelMap& map){

    Creature* c = &creatures[i];

    c->damageTics++;

    if (c->damageTics > 10){
        c->damageTics = 0;

        const float radius = c->radius/2.f;

        int x1 = (c->pos.v[0] - radius) / 32;
        int y1 = (c->pos.v[1] - radius) / 32;
        int x2 = (c->pos.v[0] + radius) / 32;
        int y2 = (c->pos.v[1] + radius) / 32;

        int tileArray[20];
        int count = 0;

        for (int i = y1; i <= y2; ++i)
        {
            for (int j = x1; j <= x2; ++j)
            {
                tileArray[count] = map.getTerrainType(j, i);
                count++;
           
                /*if (c->controled && c->isWarrior)
                {
                    switch (tileArray[count])
                    {
                        case ET_ICE: printf("ICE "); break;
                        case ET_GROUND: printf("GROUND "); break;
                        case ET_WATER: printf("WATER "); break;
                        case ET_GRASS: printf("GRASS "); break;
                        case ET_LAVA: printf("LAVA "); break;
                    }
                }*/
            }

        }


        int hpDifference = 0;

        for (int i = 0; i < count; ++i)
        {
            hpDifference += creatures[i].iTerrainBonuses[tileArray[i]];
        }

        hpDifference = hpDifference / count;

        /*if (c->controled && c->isWarrior)
        {
            printf("| %d %d %d %d x %f y %f rad %f hpDiff %d\n", x1, y1, x2, y2, c->pos.v[0], c->pos.v[1], radius, hpDifference);
        }*/


        creatures[i].hp += hpDifference;

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
    for (unsigned i = 0; i < creatures.count(); i++)
    {
        if ((creatures[i].race == race)&&(!creatures[i].dead)){
            result++;
        }
    }
    return result;
}

int Population::FemaleCount(unsigned race)
{
    int result = 0;
    for (unsigned i = 0; i < creatures.count(); i++){
        if ((creatures[i].race == race)&&(!creatures[i].dead) && (creatures[i].isFemale)){
            result++;
        }
    }
    return result;

}
//-----------------------------
void Population::create(unsigned race1, unsigned race2, SoundSystem& ss){

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
        c.attachBuffer(ss, 1, 0);
        c.attachBuffer(ss, 2, 1);
        creatures.add(c);
    }

}


void Population::Fertilize(unsigned race)
{
    for (unsigned i = 0; i < creatures.count(); ++i)
    {
        Creature* c = &creatures[i];

        if ((race == c->race) && (!c->dead))
        {
            if (c->isFemale)
            {
                c->gaveBirth = false;
            }
            else
            {
                c->procreationCount = 0;
            }

            return;
        }
    }

}

void Population::SpawnAFemale(unsigned race)
{
    for (unsigned i = 0; i < creatures.count(); ++i)
    {
        Creature* c = &creatures[i];

        if ((c->isFemale == false) && (race == c->race) && (!c->dead))
        {
            c->isFemale = true;
            return;
        }
    }

}

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

