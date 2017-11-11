#ifndef _CPP_SINGLETON
#define _CPP_SINGLETON


#include "audio/SoundSystem.h"
#include "audio/OggStream.h"
#include "TextureLoader.h"
#include "Vectors.h"
#include "SysConfig.h"
#include "TouchData.h"
#include "externals.h"
#include "gui/Button.h"
#include "LevelMap.h"
#include "Creature.h"
#include "Population.h"
#include "Player.h"
#include "Meteor.h"
#include "PowerUp.h"

enum GameStates {
    GAME,
    SELECTRACE,
    TITLE
};

enum GameModes {
    ONE,
    TWO
};

static const int kiScreenWidth = 640;
static const int kiScreenHeight = 480;

struct Joystick{
    Vector3D pos;
    float radius;
    
    Joystick(){
        pos = Vector3D(70, 250, 0);
        radius = 50.0;
    }
};


class Singleton{

public:
    static Singleton* GetInstance();
    void loadCfg();
    void init(int width = kiScreenWidth, int height = kiScreenHeight);
    void logic();
    void render();
    void destroy();

private:
    Singleton()
    : m_fImpactProgress(0.f)
    {
        gamestate = TITLE;
        Exit = false;
        alpha = 0.0f;
        fadeOut = false;
        joyPressed = false;
        acc = 24.0f;

        ChangeVolume = false;
        MusicVolume = 0.0f;
        PlayMusic = false;
        StopMusic = false;
        PlayNewSong = false;


        p[0].activeCreature = 0;
        p[1].activeCreature = 5;

        Accumulator = 0;
        DT = 1000.0f/60.0f/1000.0f;

        secondsUntilImpact = timeUntilImpact;

        startImpact = false;
        launchFireBall = false;
        showWinner = false;
        draw = false;
        winnerRace = 0;
        winnerClick = false;
        activeButton = 0;

        OnePlayer_button.set(470, 400, 128, 32);
        TwoPlayer_button.set(470, 440, 128, 32);

        drawDebugStuff = false;

    }

    void setMusicVolume(float vol);
    void playMusic();
    void playNewSong(const char* path);

    void drawMainMenu();
    void drawGame();
    void gameLogic();

    void UpdateImpact(float fDeltaTime);
    void SpawnRandomPowerup();

    void drawSelectRace();
    void DrawVictoryDialog();
    void selectRaceLogic();


    void SinglePlayerAI(int iMaxAreaX, int iMaxAreaY, float speed);

    void whoWon();
    void mainMenuLogic();

    void resetGame();

    int FPS();

    void ControlCreature(Player& player, const int* keyIndexes, Creature* playerCreature);

public:
    bool Exit;
    
    bool ChangeVolume;
    float MusicVolume;
    bool PlayMusic;
    bool PlayNewSong;
    bool StopMusic;

    float DeltaTime;
    long TimeTicks;
    float Accumulator;
    float DT;


    SoundSystem ss;
    OggStream music;
    
    PicsContainer pics;


    time_t start;
    double m_timeDiff;
    double m_timeDiffOld;
    bool launchFireBall;
    int secondsUntilImpact;
    bool startImpact;

    bool showWinner;
    int winnerRace;
    bool draw;


    SystemConfig sys;
    
    Vector3D accelerometer;
    Vector3D gamepad; //input from real device

   
    Button OnePlayer_button;
    Button TwoPlayer_button;
    int activeButton;

    Player p[2];

    char DocumentPath[255];  
    char songName[255];
  
    LevelMap Mapas;
    Population Creatures;

    Joystick joystick;
    TouchData touches;
    unsigned char Keys[20];
    unsigned char OldKeys[20];
    float acc;
    GameStates gamestate;
    GameModes gamemode;
    bool fadeOut;
    float alpha;
    bool winnerClick;
    bool joyPressed;
    unsigned globalKEY;

    bool drawDebugStuff;

private:
    static Singleton* m_Instance;
    Meteor            m_Meteor;
    PowerUpArray      m_PowerUps;
    Vector3D          m_mapOffset;
    float             m_fImpactProgress;
};

#endif //_CPP_SINGLETON
