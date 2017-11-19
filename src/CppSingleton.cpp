#include <wchar.h>
#include "CppSingleton.h"
#include "Utils.h"
#include "gui/Text.h"
#include <cmath>
#include <ctime>


Singleton* Singleton::m_Instance = 0;

Singleton* Singleton::GetInstance()
{
    if (m_Instance == 0)
    {
        m_Instance = new Singleton();
    }

    return m_Instance;
}

void Singleton::setMusicVolume(float vol){
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    MusicVolume = vol;
    ChangeVolume =true;
#else
    music.setVolume(vol);
#endif
}
//------------------------
void Singleton::playMusic(){
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    PlayMusic = true;
#else
    music.playback();
#endif
}
//---------------------
void Singleton::playNewSong(const char * path){
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    PlayNewSong = true;
    sprintf(songName, "data/%s.m4a", path);
#else
    music.stop();
    music.release();
    sprintf(songName, "data/%s.ogg", path);
    music.open(songName);
    music.playback();
#endif
    
}
//-----------------------
void Singleton::loadCfg(){

    char megabuf[255];
    sprintf(megabuf, "%s/settings.cfg", DocumentPath);
    if (!sys.load(megabuf)){
        puts(megabuf);
        sys.write(megabuf);
    }

}
//---------------------
void Singleton::init(int width, int height){
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0.0, (GLfloat) width, (GLfloat) height, 0.0, 400, -400);
    
        
    if (!pics.load("data/pics/list.txt"))
        puts("can't find list");
    
    ss.init(0);
    Vector3D p(0,0,0);
    ss.setupListener(p.v, p.v);
    ss.loadFiles("data/sfx/", "audio.txt");
#if !(TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR)
    music.open("data/music.ogg");
#endif
    

    setMusicVolume(sys.musicVolume);
    ss.setVolume(sys.sfxVolume);
    playMusic();


   }

void Singleton::drawPlayersView(const Player& p)
{
    Mapas.draw(pics, 1, 
               p.mapOffset.v[0], p.mapOffset.v[1],
               1.0f,
               p.viewPortSize.v[0], p.viewPortSize.v[1]);

    m_PowerUps.Render(pics, 20, 
                      p.mapOffset.v[0], p.mapOffset.v[1],
                      p.viewPortSize.v[0], p.viewPortSize.v[1]);
    Creatures.draw(pics, p.mapOffset.v[0], p.mapOffset.v[1],
                   p.viewPortSize.v[0], p.viewPortSize.v[1]);

    if ((launchFireBall) && (!startImpact))
    {
        m_Meteor->Render(pics, p.mapOffset.v[0], p.mapOffset.v[1]);
    }

    if (startImpact)
    {

        float fAlpha = 1.f - m_fImpactProgress + 0.2f;
        if (fAlpha > 1.f)
        {
            fAlpha = 1.f;
        }

        COLOR impactColor(1, 0, 0, fAlpha);

        pics.draw(15,
                  (Mapas.width * 32) / 2 + p.mapOffset.v[0], 
                  (Mapas.height * 32) / 2 + p.mapOffset.v[1],
                  0, true, 
                  m_fImpactProgress,
                  m_fImpactProgress, 0, 
                  impactColor,
                  impactColor);
    }

    pics.drawBatch(666);


}


//----------------------------
void Singleton::drawGame(){

    drawPlayersView(p[0]); 

    if (gamemode == TWO)
    {
        float y1 = p[0].mapOffset.v[1];
        float y2 = p[1].mapOffset.v[1];

        if (fabs(y1 - y2) > 10)
        {

            p[0].viewPortPos = Vector3D(kiScreenWidth/2.f, 64.f, 0.f);
        }
        else
        {
            p[0].viewPortPos = Vector3D(0.f, 64.f, 0.f);
        }

        if (fabs(y1 - y2) > 10)
        {
            glScissor(0, 0, kiScreenWidth/2, kiScreenHeight);
            glEnable(GL_SCISSOR_TEST);
        
            drawPlayersView(p[1]);

            pics.drawBatch(666);

            glDisable(GL_SCISSOR_TEST);
            pics.draw(-1, 319.5, 64, 0, false, 1.5, 480, 0, COLOR(1,1,0,1), COLOR(1,1,0,1));
        }
    }


    char buf[50];
    int timeleft =  secondsUntilImpact - m_timeDiff;
    if (timeleft <= 0 || launchFireBall)
    {
        timeleft = 0;
    }
    pics.draw(16, 0, 0, 0);

    pics.draw(17, 64, 0, p[0].raceIndex, false, -1, 1);
    pics.draw(17, kiScreenWidth - 64, 0, p[1].raceIndex, false, 1, 1);


    unsigned race1 = _RaceIndex[p[0].raceIndex];
    unsigned race2 = _RaceIndex[p[1].raceIndex];
    int raceCount1 = Creatures.countByRace(race1);
    int raceCount2 = Creatures.countByRace(race2);
    int p1Females = Creatures.FemaleCount(race1);
    int p2Females = Creatures.FemaleCount(race2);
    
    pics.draw(19, 80, 25, 1);
    pics.draw(19, 100, 25, 0);
    pics.draw(19, 528, 25, 1);
    pics.draw(19, 548, 25, 0);

    sprintf(buf, "%d", p1Females);
    WriteShadedText(80, 40, pics, 0, buf);
    sprintf(buf, "%d", p2Females);
    WriteShadedText(528, 40, pics, 0, buf);
    sprintf(buf, "%d", raceCount1 - p1Females);
    WriteShadedText(100, 40, pics, 0, buf);
    sprintf(buf, "%d", raceCount2 - p2Females);
    WriteShadedText(548, 40, pics, 0, buf);


    //300px max
    //120 secs max
    pics.draw(-1, 182, 15, 0, false, (timeleft / 1.2) * 3, 25, 0, COLOR(1,0,0,0.8), COLOR(1,0,0,0.8));

    if (showWinner)
    {
        DrawVictoryDialog();
    }


    sprintf(buf, "%d", raceCount1);
    WriteShadedText(80, 6, pics, 0, buf, 1.5f, 1.5f);
    sprintf(buf, "%d", raceCount2);
    WriteShadedText(548, 6, pics, 0, buf, 1.5f, 1.5f);


    pics.drawBatch(666);
}

void Singleton::UpdateImpact(float fDeltaTime){

    if (m_fImpactProgress < 1.0f)
    {
        m_fImpactProgress += fDeltaTime * 0.35f;
        Mapas.affectMap(320, 240, 404 * m_fImpactProgress);
    }
    else
    {
        whoWon();
    }

}

void Singleton::SpawnRandomPowerup()
{
    m_timeDiffOld = m_timeDiff;
    //printf("Spawning powerup %d %d\n", rand()%Mapas.width, rand()%Mapas.height);
    PowerUp newP;
    newP.pos = Vector3D(Random(16, Mapas.width * 32 - 16),
                        Random(16, Mapas.height * 32 - 16),
                        0.f);

        int iRandomValue = Random(0, 100);
        newP.type = (iRandomValue * iRandomValue) / (10000 / PT_COUNT); //square distribution
        newP.radius = 16.f;
        m_PowerUps.m_PowerUps.add(newP);
}

void Singleton::whoWon()
{
    if (showWinner == true)
    {
        return;
    }

    int raceCount1 = 0;
    int raceCount2 = 0;
    unsigned race1 = _RaceIndex[p[0].raceIndex];
    unsigned race2 = _RaceIndex[p[1].raceIndex];

    raceCount1 = Creatures.countByRace(race1);
    raceCount2 = Creatures.countByRace(race2);

    if (raceCount2 > raceCount1)
        winnerRace = race2;
    if (raceCount1 > raceCount2)
        winnerRace = race1;
    if (raceCount1 == raceCount2)
    {
        draw = true;
    }

    if ((raceCount2 < 1) || (raceCount1 < 1) || m_fImpactProgress > 1.f)
    {
        showWinner = true;
    }
}

void Singleton::gameLogic()
{
    time_t now;
    time(&now);
    m_timeDiff = difftime(now, start);

    Mapas.animateTiles();

    Creatures.Update(DeltaTime,
                     Mapas,
                     ss,
                     m_PowerUps,
                     secondsUntilImpact,
                     kiScreenWidth, kiScreenHeight,
                     p[0].activeCreature, p[1].activeCreature,
                     startImpact, showWinner);

    m_PowerUps.Update(DeltaTime);
    
   

    if ((startImpact)&&(!showWinner))
    {
        UpdateImpact(DeltaTime);
    }

    if ((m_timeDiff - m_timeDiffOld > 2)&&(!startImpact))
    {
        SpawnRandomPowerup();
    }


    if (launchFireBall && !startImpact)
    {
        m_Meteor->AnimateFireBall();
        m_Meteor->AnimateTrail();

        if (m_Meteor->Update(DeltaTime))
        {
            startImpact = true;
        }
    }

    if ((int)m_timeDiff >= secondsUntilImpact){
        if(!showWinner)
        {
            launchFireBall = true;
        }
    }

    if ((OldKeys[7])&&(!Keys[7])){
        Creatures.nextActive(p[0].activeCreature, p[0].activeCreature);
    }

    if ((OldKeys[6])&&(!Keys[6])){
        Creatures.nextActive(p[1].activeCreature, p[1].activeCreature);
    }


    if ((winnerClick)&&(!Keys[4])){
        winnerClick = false;
        gamestate = SELECTRACE;
    }

    if (showWinner){
        if (Keys[4])
            winnerClick = true;
    }


    if (!startImpact){
        
        Creature* c = 0;
        c = Creatures.get(p[0].activeCreature);
        const int p1KeyIndexes[] = {8, 9, 10, 11, 4, 14};
        ControlCreature(p[0], p1KeyIndexes, c);
                
        if (gamemode == TWO)
        {
            c = Creatures.get(p[1].activeCreature);
            const int p2KeyIndexes[] = {0, 1, 2, 3, 5, 13};
            ControlCreature(p[1], p2KeyIndexes, c);
        }
        else
        {
            SinglePlayerAI(2.f);
        }

    }
}

void Singleton::drawMainMenu()
{
    pics.draw(11,0,0);
    pics.draw(13,300,50, 0, false, 0.5f, 0.5f);
    pics.draw(5, 440, activeButton * 40 + 416, 1, true);
    OnePlayer_button.draw(pics, 18, 0);
    TwoPlayer_button.draw(pics, 18, 1);
    pics.drawBatch(666);
}
//-------------------
void Singleton::render(){
    glClear(GL_COLOR_BUFFER_BIT);
    switch(gamestate){
        case GAME : drawGame(); break;
        case SELECTRACE: drawSelectRace(); break;
        case TITLE: drawMainMenu(); break;
    }

    if (drawDebugStuff)
    {
        char buf[50];
        sprintf(buf, "FPS: %d", FPS());
        WriteShadedText(0, kiScreenHeight - 20, pics, 0, buf);
        sprintf(buf, "DeltaTime: %f", DeltaTime);
        WriteShadedText(0, kiScreenHeight - 40, pics, 0, buf);


        if (gamestate == GAME)
        {
            int timeleft =  secondsUntilImpact - m_timeDiff;
            if (timeleft <= 0 || launchFireBall)
            {
                timeleft = 0;
            }
            sprintf(buf,"%d", timeleft);
            WriteShadedText(320, 10, pics, 0, buf);
        }

        pics.drawBatch(666);
    }

}
//---------------------
void Singleton::resetGame(){
    gamestate = GAME;


    p[0].raceSelected = false;
    p[1].raceSelected = false;

    draw = false;
    showWinner = false;
    winnerRace = 0;

    Creatures.destroy();
    Mapas.generate(MaxMapWidth, MaxMapHeight);


    if (m_Meteor)
    {
        delete m_Meteor;
    }

    m_Meteor = new Meteor();

    m_Meteor->AttachSoundSources(ss, 0);
    m_Meteor->SetDestination((Mapas.width * 32) / 2, (Mapas.height * 32) / 2);

    m_PowerUps.m_PowerUps.destroy();

    
    Creatures.create(p[0].raceIndex, p[1].raceIndex, ss);

    p[0].activeCreature = 0;
    p[0].Id = 1;
    p[1].activeCreature = 5;
    p[1].Id = 2;
    Creatures.get(p[0].activeCreature)->controled = true;
    Creatures.get(p[1].activeCreature)->controled = true;
    time(&start);

    secondsUntilImpact = timeUntilImpact;
    startImpact = false;
    launchFireBall = false;
    m_timeDiffOld = 0;
    m_timeDiff = 0;

    m_fImpactProgress = 0.f;

    p[0].viewPortSize = Vector3D(kiScreenWidth, kiScreenHeight, 0.f);
    p[0].viewPortPos = Vector3D(0.f, 64.f, 0.f);

    if (gamemode == TWO)
    {
        p[0].viewPortSize = Vector3D(kiScreenWidth, kiScreenHeight, 0.f);
        p[0].viewPortPos = Vector3D(kiScreenWidth/2.f, 64.f, 0.f);

        p[1].viewPortSize = Vector3D(kiScreenWidth/2, kiScreenHeight, 0.f);
        p[1].viewPortPos = Vector3D(0.f, 64.f, 0.f);
    }

}
//------------------------
void Singleton::selectRaceLogic(){
  
    if ((!Keys[12])&&(OldKeys[12]))
        gamestate = TITLE;

    if ((!p[0].raceSelected )&&(gamemode == TWO)){
        if ((OldKeys[1])&&(!Keys[1])){
            if (p[0].raceIndex < 3)
                p[0].raceIndex++;
        }

        if ((OldKeys[0])&&(!Keys[0])){
            if (p[0].raceIndex > 0)
                p[0].raceIndex--;
        }

        if (Keys[5]){
            if (!((p[1].raceSelected)&&(p[1].raceIndex == p[0].raceIndex)))
            p[0].raceSelected = true;
        }
    }
//-------
    if (!p[1].raceSelected){
        if ((OldKeys[9])&&(!Keys[9])){
            if (p[1].raceIndex < 3)
                p[1].raceIndex++;
        }

        if ((OldKeys[8])&&(!Keys[8])){
            if (p[1].raceIndex > 0)
                p[1].raceIndex--;
        }

        if (Keys[4]){
            if (!((p[0].raceSelected)&&(p[1].raceIndex == p[0].raceIndex)))
                p[1].raceSelected = true;
        }
    }


    if (gamemode == TWO){
        if ((p[1].raceSelected) && (p[0].raceSelected)){
            resetGame();
        }
    }
    else{
        if ((p[1].raceSelected)){
            p[0].raceIndex = rand()%4;
            while (p[0].raceIndex == p[1].raceIndex)
                p[0].raceIndex = rand()%4;
            resetGame();
        }

    }
}

void Singleton::SinglePlayerAI(float speed)
{
    Creature* c = Creatures.get(p[1].activeCreature);

    if (!c->haveDir)
    {
        float shortestDistance = 10000.0f;
        Vector3D dir = Vector3D(1,0,0);
        //finds direction to the nearest creature
        for (unsigned i = 0; i < Creatures.count(); i++)
        {
            Creature * mate = Creatures.get(i);

            if ((c->canProcreateWith(mate)) && (!mate->dead) &&
                (mate->race == c->race) && (p[1].activeCreature != i) && (mate->radius > 15.5f))
            {
                mate->pos.v[2] = 0;
                c->pos.v[2] = 0;
                Vector3D d = mate->pos - c->pos;
                float len = d.length();

                if (len < shortestDistance)
                {
                    shortestDistance = len;
                    dir = d;
                }

                c->haveDir = true;
            }
        }

        if (c->haveDir)
        {
            dir.normalize();
            c->dir = dir;
        }
        else //look for powerups
        {
            Vector3D pDir;
            int iFemaleCount = Creatures.FemaleCount(c->race);
            int iRaceCount = Creatures.countByRace(c->race);

            if (m_PowerUps.FindNearestPowerUp(PT_WARRIOR, c->pos, pDir))
            {
                pDir.normalize();
                c->dir = pDir;
                c->haveDir = true;
            }
            else if (m_PowerUps.FindNearestPowerUp(PT_BUILDER, c->pos, pDir))
            {
                pDir.normalize();
                c->dir = pDir;
                c->haveDir = true;
            }
            else if ((m_PowerUps.FindNearestPowerUp(PT_FEMALE, c->pos, pDir)) && (iFemaleCount < iRaceCount))
            {
                pDir.normalize();
                c->dir = pDir;
                c->haveDir = true;
            }
            else if (m_PowerUps.FindNearestPowerUp(PT_FERTILITY, c->pos, pDir) && (iFemaleCount > 0))
            {
                pDir.normalize();
                c->dir = pDir;
                c->haveDir = true;
            }

        }
    }
    else
    {
        c->Move(speed, Mapas.width * 32, Mapas.height * 32);
        c->haveDir = false;
    }

    Creatures.interact(p[1].activeCreature, ss);

    if ((c->procreationCount >= c->maxProcreationCount) || (c->gaveBirth) || (c->isWarrior) || (c->isBuilder))
    {
        Creatures.nextActive(p[1].activeCreature, p[1].activeCreature);
    }

   
}
//------------------------
void Singleton::drawSelectRace(){

    int y = 100;

    pics.draw(11,0,0);
    pics.draw(-1, 0, 0, 0, false, 640, 480, 0,
              COLOR(0.4f,0.4f,0.4f, 0.7f), COLOR(0.4f,0.4f,0.4f, 0.7f));

    WriteShadedText(200, 20, pics, 0, "Choose your species:");


    if (gamemode == TWO){
        pics.draw(-1, 50, p[0].raceIndex*50 + y, 0, false, 270, 50, 0,
                  COLOR(1,0,0,1), COLOR(1,0,0,0));
        WriteShadedText(100, 5 + y, pics, 0, RaceNames[0]);
        WriteShadedText(180, 20 + y, pics, 0, "thrives on grassy plains", 0.8f, 0.8f);
        WriteShadedText(100, 55 + y, pics, 0, RaceNames[1]);
        WriteShadedText(180, 70 + y, pics, 0, "preffers ice", 0.8f, 0.8f);
        WriteShadedText(100, 105 + y, pics, 0,RaceNames[2]);
        WriteShadedText(180, 120 + y, pics, 0, "strong in water", 0.8f, 0.8f);
        WriteShadedText(100, 155 + y, pics, 0, RaceNames[3]);
        WriteShadedText(180, 170 + y, pics, 0, "love barren land", 0.8f, 0.8f);

        pics.draw(-1, 640-270-50, p[1].raceIndex * 50 + y, 0, false, 270, 50, 0, 
                  COLOR(0,0,1,1), COLOR(0,0,1,0));

        WriteShadedText(400, 5 + y, pics, 0, RaceNames[0]);
        WriteShadedText(400, 55 + y, pics, 0, RaceNames[1]);
        WriteShadedText(400, 105 + y, pics, 0, RaceNames[2]);
        WriteShadedText(400, 155 + y, pics, 0, RaceNames[3]);
    }
    else{

        pics.draw(-1, 185, p[1].raceIndex * 50 + y, 0, false, 270, 50, 0, 
                  COLOR(0,0,1,1), COLOR(0,0,1,0));

        WriteShadedText(200, 5 + y, pics, 0, RaceNames[0]);
        WriteShadedText(200, 55 + y, pics, 0, RaceNames[1]);
        WriteShadedText(200, 105 + y, pics, 0, RaceNames[2]);
        WriteShadedText(200, 155 + y, pics, 0, RaceNames[3]);
        WriteShadedText(200, 20 + y, pics, 0, "thrives on grassy plains", 0.8f, 0.8f);
        WriteShadedText(200, 70 + y, pics, 0, "preffers ice", 0.8f, 0.8f);
        WriteShadedText(200, 120 + y, pics, 0, "strong in water", 0.8f, 0.8f);
        WriteShadedText(200, 170 + y, pics, 0, "love barren land", 0.8f, 0.8f);
    }



    pics.drawBatch(666);

}

void Singleton::DrawVictoryDialog()
{
    pics.draw(-1,120,140,0, false,400,200,0, COLOR(0.4,0.4,0.4,0.8), COLOR(0.4, 0.4, 0.4, 0.8));
    if ((!draw)&&(winnerRace))
    {

        int iRaceNameIndex = 0;
        switch(winnerRace){

            case 2:{
                        iRaceNameIndex = 1;
                        pics.draw(8, 125, 145, 0, false);
                   } break;
            case 3:{
                        iRaceNameIndex = 0;
                        pics.draw(7, 125, 145, 0, false);
                   }break;
            case 5:{ 
                        iRaceNameIndex = 2;
                        pics.draw(10, 125, 145, 0, false);
                    }break;
            case 6:{
                        iRaceNameIndex = 3;
                        pics.draw(9, 125, 145, 0, false);
                    };break;
        }

        char finalBuff[255];
        sprintf(finalBuff, "%s have won!", RaceNames[iRaceNameIndex]);
        WriteShadedText(300, 260, pics, 0, finalBuff);

    }
    else
    {
        WriteShadedText(320-100, 230, pics, 0, "Draw!");
    }

}

//---------------------
void Singleton::mainMenuLogic(){

    COLOR dark = COLOR(0.5f, 0.5f, 0.5f);
    COLOR white = COLOR(1.0f, 1.0f, 1.0f);

    if ((!Keys[8])&&(OldKeys[8])){
        activeButton--;
        if (activeButton < 0)
            activeButton = 1;
    }
    
    if ((!Keys[9])&&(OldKeys[9])){
        activeButton++;
        if (activeButton > 1)
            activeButton = 0;
    }

    if ((!Keys[12])&&(OldKeys[12]))
        Exit = true;


    if ((!Keys[4])&&(OldKeys[4])){
        gamestate = SELECTRACE;
        if (activeButton == 0)
        {
            gamemode = ONE;
        }
        else 
        {
            gamemode = TWO;
        }
    }

    switch(activeButton){
        case 0:{
            OnePlayer_button.c = dark;
            TwoPlayer_button.c = white;
        }break;
        case 1:{
            TwoPlayer_button.c = dark;
            OnePlayer_button.c = white;
        }break;
    }

}
//---------------------
void Singleton::logic(){
#if !(TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR)
    if (music.playing())
        music.update();
#endif
    
    switch(gamestate){
        case GAME: gameLogic(); break;
        case SELECTRACE: selectRaceLogic(); break;
        case TITLE: mainMenuLogic(); break; 
    }
    
    touches.oldDown.destroy();
    for (unsigned long i = 0; i < touches.down.count(); i++ ){
        Vector3D v = touches.down[i];
        touches.oldDown.add(v);
    }
    touches.up.destroy();
    touches.down.destroy();
    touches.move.destroy();
}
//-------------------------
void Singleton::destroy(){
    
    touches.up.destroy();
    touches.down.destroy();
    touches.move.destroy();
    touches.oldDown.destroy();
    
#if (!TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR)
    music.stop();
    music.release();
#endif

    Creatures.destroy();
    Mapas.destroy();

    if (m_Meteor)
    {
        delete m_Meteor;
    }

    ss.exit();
    pics.destroy();
}

int Singleton::FPS()
{
    static int ctime = 0, FPS = 0, frames = 0, frames0 = 0;
    if ((int)TimeTicks >= ctime) {
        FPS = frames - frames0;
        ctime = (int)TimeTicks + 1000;
        frames0 = frames;
    }
    frames = frames+1;
    return FPS;
}

void Singleton::ControlCreature(Player& player, const int* keyIndexes, Creature* playerCreature)
{
    const float speed = 2.f;

    if (playerCreature->procreating == false)
    {

        playerCreature->dir.v[0] = 0;
        playerCreature->dir.v[1] = 0;

        if (Keys[keyIndexes[0]])
        {
            playerCreature->dir.v[1] = -1.f;
        }

        if (Keys[keyIndexes[1]])
        {
            playerCreature->dir.v[1] = 1.f;
        }

        if (Keys[keyIndexes[2]])
        {
            playerCreature->dir.v[0] = -1.f;
        }

        if (Keys[keyIndexes[3]])
        {
            playerCreature->dir.v[0] = 1.f;
        }

        //Vector3D oldPos = playerCreature->pos;
        playerCreature->Move(speed, Mapas.width * 32, Mapas.height * 32);


        const float fTileSize = 32.f;


        float OffsetX = (player.viewPortSize.v[0] - player.viewPortPos.v[0])/ 2.f - playerCreature->pos.v[0] + player.viewPortPos.v[0];

        if ((OffsetX > player.viewPortPos.v[0]) || 
            (Mapas.width * fTileSize <= (player.viewPortSize.v[0] - player.viewPortPos.v[0])))
        {
            OffsetX = player.viewPortPos.v[0];
        }

        if (OffsetX < (Mapas.width * fTileSize) * -1.f + player.viewPortSize.v[0])
        {
                OffsetX = (Mapas.width * fTileSize) * -1.f + player.viewPortSize.v[0];
        }

        float OffsetY = (player.viewPortSize.v[1] - player.viewPortPos.v[1]) / 2.f - playerCreature->pos.v[1];

        if ((OffsetY > player.viewPortPos.v[1])||
            (Mapas.height * fTileSize <= (player.viewPortSize.v[1] - player.viewPortPos.v[1])))
        {
            OffsetY = player.viewPortPos.v[1];
        }

        if (OffsetY < 0)
        {
            OffsetY = 0.f;
        }

        player.mapOffset = Vector3D( OffsetX, OffsetY, 0);


        if ((OldKeys[keyIndexes[4]]) && (!Keys[keyIndexes[4]]))
        {
            Creatures.interact(player.activeCreature, ss);

            if (playerCreature->procreating)
            {
                Creatures.nextActive(player.activeCreature, player.activeCreature);
            }

        }

        if ((OldKeys[keyIndexes[5]]) && (!Keys[keyIndexes[5]]))
        {
            playerCreature->terraform(Mapas);
        }
    }

}

