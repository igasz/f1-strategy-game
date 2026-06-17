#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <math.h>

#define PI 3.14159265f

// zmienne globalne z kolorami
extern const Color bgDark;
extern const Color neonCyan;
extern const Color neonGreen;
extern const Color neonRed;
extern const Color borderGray;
extern const Color textDim;
extern const Color softColor;
extern const Color mediumColor;
extern const Color hardColor;
extern const Color wetColor;

// struktury
// opony
typedef enum { SOFT, MEDIUM, HARD, WET } TyreType;

// pogoda
typedef enum { SUNNY, RAINY } WeatherState;

typedef struct {
    Vector2 position;      // Pozycja X i Y
    Color teamColor;       // Kolor zespołu
    float speed;           // Prędkość poruszania się
    float currentAngle;    // Kąt na torze
    TyreType tyre;         // Rodzaj założonej opony
    float tyreWear;        // Zużycie opon
    int pitTimer;          // odliczanie w picie
    int laps;              // Licznik okrążeń
    bool finished;         // Czy bolid dojechał do mety?
    bool pitRequested;     // box box
    float speedBase;       // Indywidualna prędkość bolidu
    float lineOffset;      // Przesunięcie na torze (żeby nie jechali gęsiego)
    const char* name;      // Nazwa zespołu
    float finishTime;
    int spinTimer;         // poślizg
} Bolid;

typedef struct {
    Bolid cars[10]; // bolidy
    int standings[10];
    int gameState;  // stan
    int playerID;
    int startTimer;
    int lightsOutDelay;
    int totalLaps;
    float raceTime;
    float postRaceTimer;
    TyreType startingTyre;
    TyreType nextTyre;
    bool closeGame;
    
    Vector2 trackCenter;
    float trackRadiusX;
    float trackRadiusY;
    Font stratFont;

    Rectangle teamBtns[10];
    Rectangle menuSoft, menuMed, menuHard, startRaceBtn;
    Rectangle boxButton, btnSoft, btnMedium, btnHard;
    Rectangle btnPlayAgain, btnExit;

    WeatherState currentWeather; // pogoda
    Rectangle btnWeatherToggle; // zmiana pogody
    Rectangle menuWet;
    Rectangle btnWet;
} Game;


// funkcje
void InitGame(Game* g);
void StartSession(Game* g);
void SortStandings(Game* g);
void UpdateMenu(Game* g);
void DrawMenu(Game* g);
void UpdateRace(Game* g);
void DrawRace(Game* g);
void UpdateEndScreen(Game* g);
void DrawEndScreen(Game* g);

#endif