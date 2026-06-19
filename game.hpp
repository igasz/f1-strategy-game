#pragma once

#include "raylib.h"
#include <cmath>
#include <string>
#include <array>

constexpr float M_PI_F = 3.14159265f;

// Przestrzeń nazw dla kolorów
namespace GameColors {
    inline constexpr Color bgDark = { 18, 18, 20, 255 };
    inline constexpr Color neonCyan = { 0, 243, 255, 255 };
    inline constexpr Color neonGreen = { 183, 247, 0, 255 };
    inline constexpr Color neonRed = { 255, 180, 171, 255 };
    inline constexpr Color borderGray = { 34, 34, 38, 255 };
    inline constexpr Color textDim = { 185, 202, 203, 255 };
    inline constexpr Color softColor = { 255, 80, 80, 255 };
    inline constexpr Color mediumColor = { 255, 200, 0, 255 };
    inline constexpr Color hardColor = { 220, 220, 220, 255 };
    inline constexpr Color wetColor = { 0, 150, 255, 255 };
}

// Silnie typowane enumy
enum class TyreType { SOFT, MEDIUM, HARD, WET };
enum class WeatherState { SUNNY, RAINY };

struct Bolid {
    Vector2 position;
    Color teamColor;
    float speed;
    float currentAngle;
    TyreType tyre;
    float tyreWear;
    int pitTimer;
    int laps;
    bool finished;
    bool pitRequested;
    float speedBase;
    float lineOffset;
    std::string name;
    float finishTime;
    int spinTimer;
};

class Game {
public:
    Game();
    ~Game();

    void Run(); // Główna pętla gry

private:
    void InitGame();
    void StartSession();
    void SortStandings();
    void UpdateMenu();
    void DrawMenu();
    void UpdateRace();
    void DrawRace();
    void UpdateEndScreen();
    void DrawEndScreen();

    // Zmienne instancji
    std::array<Bolid, 10> cars;
    std::array<int, 10> standings;
    
    int gameState;
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

    std::array<Rectangle, 10> teamBtns;
    Rectangle menuSoft, menuMed, menuHard, startRaceBtn;
    Rectangle boxButton, btnSoft, btnMedium, btnHard;
    Rectangle btnPlayAgain, btnExit;

    WeatherState currentWeather;
    Rectangle btnWeatherToggle;
    Rectangle menuWet;
    Rectangle btnWet;
};