#include <wchar.h>
#include "CppSingleton.h"
#include "Utils.h"
#include "gui/Text.h"
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

//----------------------------
void Singleton::drawGame(){

    Mapas.draw(pics, 1, 0, 0, 1.0f, kiScreenWidth, kiScreenHeight);
    m_PowerUps.Render(pics, 20);
    Creatures.draw(pics);

    if ((launchFireBall)&&(!startImpact))
    {
        m_Meteor.Render(pics);
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
                  (Mapas.width * 32) / 2, 
                  (Mapas.height * 32) / 2,
                  0, true, 
                  m_fImpactProgress,
                  m_fImpactProgress, 0, 
                  impactColor,
                  impactColor);
    }

    char buf[50];
    int timeleft =  secondsUntilImpact - m_timeDiff;
    if (timeleft <= 0 || launchFireBall)
    {
        timeleft = 0;
    }
    pics.draw(-1, 100, 0, 0, false, 440, 40,0, COLOR(0.4,0.4,0.4,0.8), COLOR(0.4, 0.4, 0.4, 0.8));
    sprintf(buf,"Time left until impact: %d", timeleft);
    WriteShadedText(320-strlen(buf)*13/2, 10, pics, 0, buf);

    if (showWinner)
    {
        DrawVictoryDialog();
    }

    unsigned race1 = _RaceIndex[p[0].raceIndex];
    unsigned race2 = _RaceIndex[p[1].raceIndex];

    int raceCount1 = Creatures.countByRace(race1);
    int raceCount2 = Creatures.countByRace(race2);
    sprintf(buf, "%d", raceCount1);
    WriteShadedText(50, 20, pics, 0, buf);
    sprintf(buf, "%d", raceCount2);
    WriteShadedText(600, 20, pics, 0, buf);

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


    if (launchFireBall)
    {
        m_Meteor.AnimateFireBall();
        m_Meteor.AnimateTrail();

        if (m_Meteor.Update(ss))
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
            SinglePlayerAI(kiScreenWidth, kiScreenHeight, 2.f);
        }

    }
}

void Singleton::drawMainMenu()
{
    pics.draw(11,0,0);
    pics.draw(13,300,50, 0, false, 0.5f, 0.5f);
    //WriteShadedText(370, 420, pics, 0, "Press Enter to play...");
    pics.draw(16, 440, activeButton * 40 + 416, 1, true, 1.15f, 1.15f, 0, COLOR(0,0,0), COLOR(0,0,0));
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
        WriteText(0, kiScreenHeight - 20, pics, 0, buf);
        sprintf(buf, "DeltaTime: %f", DeltaTime);
        WriteText(0, kiScreenHeight - 40, pics, 0, buf);
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

    m_Meteor.Destroy();
    m_PowerUps.m_PowerUps.destroy();

    Creatures.destroy();
    Mapas.generate(MaxMapWidth, MaxMapHeight);

    Creatures.create(p[0].raceIndex, p[1].raceIndex);
    p[0].activeCreature = 0;
    p[1].activeCreature = 5;
    Creatures.get(p[0].activeCreature)->controled = true;
    Creatures.get(p[1].activeCreature)->controled = true;
    time(&start);

    secondsUntilImpact = timeUntilImpact;
    startImpact = false;
    launchFireBall = false;
    m_timeDiffOld = 0;
    m_timeDiff = 0;

    m_fImpactProgress = 0.f;

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

void Singleton::SinglePlayerAI(int iMaxAreaX, int iMaxAreaY, float speed)
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
        else //look for a female powerup
        {
            Vector3D pDir;
            int iFemaleCount = Creatures.FemaleCount(c->race);
            int iRaceCount = Creatures.countByRace(c->race);

            if ((m_PowerUps.FindNearestPowerUp(PT_FEMALE, c->pos, pDir)) && (iFemaleCount < iRaceCount))
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
        c->Move(speed, iMaxAreaX, iMaxAreaY);
        c->haveDir = false;
    }

    Creatures.interact(p[1].activeCreature);

    if ((c->procreationCount >= c->maxProcreationCount) || (c->gaveBirth))
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
            gamemode = ONE;
        else 
            gamemode = TWO;
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
    m_Meteor.Destroy();

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

        playerCreature->Move(speed, kiScreenWidth, kiScreenHeight);

        if ((OldKeys[keyIndexes[4]]) && (!Keys[keyIndexes[4]]))
        {
            Creatures.interact(player.activeCreature);

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

