#include "Game.hpp"

using namespace GameColors;

const std::array<Color, 10> teamColors = {
    Color{ 54, 113, 198, 255 },  // 0. Red Bull Racing (Deep Blue)
    Color{ 232, 0, 32, 255 },    // 1. Ferrari (Red)
    Color{ 39, 244, 210, 255 },  // 2. Mercedes (Petronas Teal)
    Color{ 255, 128, 0, 255 },   // 3. McLaren (Papaya Orange)
    Color{ 34, 89, 61, 255 },    // 4. Aston Martin (British Racing Green)
    Color{ 255, 135, 188, 255 }, // 5. Alpine (BWT Pink)
    Color{ 0, 160, 222, 255 },   // 6. Williams (Light Blue)
    Color{ 102, 146, 255, 255 }, // 7. VCARB (Blue/Silver)
    Color{ 82, 226, 82, 255 },   // 8. Kick Sauber (Neon Green)
    Color{ 255, 255, 255, 255 }  // 9. Haas (White/Red)
};

const std::array<std::string, 10> teamNames = {
    "RED BULL RACING",
    "SCUDERIA FERRARI",
    "MERCEDES-AMG",
    "MCLAREN",
    "ASTON MARTIN",
    "ALPINE",
    "WILLIAMS RACING",
    "VCARB",
    "KICK SAUBER",
    "HAAS F1 TEAM"
};

Game::Game() {
    InitGame();
}

Game::~Game() {
    UnloadFont(stratFont);
}

void Game::Run() {
    while (!WindowShouldClose() && !closeGame) {
        // Aktualizacja stanów
        if (gameState == 0) {
            UpdateMenu();
        } else if (gameState == 1 || gameState == 2) {
            UpdateRace();
        } else if (gameState == 3) {
            UpdateEndScreen();
        }

        // Rysowanie
        BeginDrawing();
        ClearBackground(bgDark); 
        
        if (gameState == 0) {
            DrawMenu();
        } else if (gameState == 1 || gameState == 2) {
            DrawRace();
        } else if (gameState == 3) {
            DrawEndScreen();
        }
        
        EndDrawing();
    }
}

void Game::InitGame() {
    stratFont = LoadFontEx("Lato-Regular.ttf", 120, 0, 0);
    totalLaps = 10;
    trackCenter = { 640.0f, 400.0f };
    trackRadiusX = 220.0f;
    trackRadiusY = 320.0f;
    
    gameState = 0; // menu
    playerID = 0;
    startingTyre = TyreType::SOFT;
    nextTyre = TyreType::SOFT;
    closeGame = false;

    // przyciski
    for (int i = 0; i < 10; i++) {
        teamBtns[i] = { 150.0f, 220.0f + (i * 45), 450.0f, 40.0f };
    }

    currentWeather = WeatherState::SUNNY; 

    btnWeatherToggle = { 720.0f, 242.0f, 370.0f, 45.0f };

    menuSoft = { 720.0f, 340.0f, 370.0f, 40.0f };
    menuMed  = { 720.0f, 390.0f, 370.0f, 40.0f };
    menuHard = { 720.0f, 440.0f, 370.0f, 40.0f };
    menuWet  = { 720.0f, 490.0f, 370.0f, 40.0f };
    
    startRaceBtn = { 720.0f, 560.0f, 370.0f, 60.0f };

    boxButton = { 980.0f, 80.0f, 260.0f, 50.0f };
    
    btnSoft   = { 970.0f, 150.0f, 60.0f, 30.0f };
    btnMedium = { 1040.0f, 150.0f, 60.0f, 30.0f };
    btnHard   = { 1110.0f, 150.0f, 60.0f, 30.0f };
    btnWet    = { 1180.0f, 150.0f, 60.0f, 30.0f };
    
    btnPlayAgain = { 1280.0f/2 - 280, 650.0f, 260.0f, 60.0f };
    btnExit      = { 1280.0f/2 + 20, 650.0f, 260.0f, 60.0f };
}

void Game::StartSession() {
    for (int i = 0; i < 10; i++) {
        cars[i].teamColor = teamColors[i];
        cars[i].name = teamNames[i];
        
        // losowanie opon
        if (i == playerID) {
            cars[i].tyre = startingTyre;
        } else {
            if (currentWeather == WeatherState::RAINY) {
                cars[i].tyre = TyreType::WET;
            } else {
                cars[i].tyre = static_cast<TyreType>(GetRandomValue(0, 2));
            }
        }
        
        // rozstawiamy
        cars[i].currentAngle = -(i * 0.08f); 
        cars[i].speedBase = 0.021f + (static_cast<float>(GetRandomValue(-10, 10)) / 10000.0f);
        
        cars[i].lineOffset = static_cast<float>(i % 3) * 12.0f; 
        cars[i].position.x = trackCenter.x + cos(cars[i].currentAngle) * (trackRadiusX + cars[i].lineOffset);
        cars[i].position.y = trackCenter.y + sin(cars[i].currentAngle) * (trackRadiusY + cars[i].lineOffset);
        
        cars[i].laps = 0;
        cars[i].pitTimer = 0;
        cars[i].finished = false;
        cars[i].pitRequested = false;
        cars[i].finishTime = 0.0f;
        cars[i].spinTimer = 0;
        cars[i].tyreWear = 100.0f;
        
        standings[i] = i; 
    }
    
    nextTyre = startingTyre; 
    raceTime = 0.0f;
    postRaceTimer = 0.0f;
    
    gameState = 1;
    startTimer = 0;
    lightsOutDelay = 240 + GetRandomValue(30, 120); 
}

void Game::SortStandings() {
    for(int j = 0; j < 9; j++) {
        for(int k = 0; k < 9 - j; k++) {
            int indexA = standings[k];     
            int indexB = standings[k+1];   

            Bolid& carA = cars[indexA];
            Bolid& carB = cars[indexB];

            float scoreA;
            float scoreB;

            if (carA.finished) {
                scoreA = 100000.0f - carA.finishTime;
            } else {
                scoreA = (carA.laps * 1000.0f) + carA.currentAngle;
            }

            if (carB.finished) {
                scoreB = 100000.0f - carB.finishTime;
            } else {
                scoreB = (carB.laps * 1000.0f) + carB.currentAngle;
            }

            if (scoreB > scoreA) {
                int temp = standings[k];
                standings[k] = standings[k+1];
                standings[k+1] = temp;
            }
        }
    }
}

void Game::UpdateMenu() {
    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < 10; i++) {
        if (CheckCollisionPointRec(mousePos, teamBtns[i]) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            playerID = i;
        }
    }

    if (CheckCollisionPointRec(mousePos, menuSoft) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        startingTyre = TyreType::SOFT;
    }
    if (CheckCollisionPointRec(mousePos, menuMed) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        startingTyre = TyreType::MEDIUM;
    }
    if (CheckCollisionPointRec(mousePos, menuHard) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        startingTyre = TyreType::HARD;
    }
    if (CheckCollisionPointRec(mousePos, menuWet) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        startingTyre = TyreType::WET;
    }

    if (CheckCollisionPointRec(mousePos, btnWeatherToggle) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentWeather = (currentWeather == WeatherState::SUNNY) ? WeatherState::RAINY : WeatherState::SUNNY;
    }

    if (CheckCollisionPointRec(mousePos, startRaceBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        StartSession();
    }
}

void Game::DrawMenu() {
    Vector2 mousePos = GetMousePosition();
    
    DrawTextEx(stratFont, "INITIALIZATION", { 150, 100 }, 40, 2, WHITE);
    DrawTextEx(stratFont, "CHOOSE YOUR TEAM AND STARTING STRATEGY", { 150, 150 }, 14, 2, textDim);

    for (int i = 0; i < 10; i++) {
        bool isHovered = CheckCollisionPointRec(mousePos, teamBtns[i]);
        Color btnBorder = isHovered ? WHITE : borderGray;
        if (playerID == i) btnBorder = neonCyan;
        
        DrawRectangleLinesEx(teamBtns[i], (playerID == i) ? 3.0f : 2.0f, btnBorder);
        DrawRectangle(teamBtns[i].x + 15, teamBtns[i].y + 10, 20, 20, teamColors[i]);
        DrawTextEx(stratFont, teamNames[i].c_str(), { teamBtns[i].x + 55, teamBtns[i].y + 12 }, 20, 2, (playerID == i || isHovered) ? WHITE : textDim);
    }

    DrawRectangleLines(680, 220, 450, 450, borderGray);
    DrawRectangle(680, 220, 450, 2, neonCyan);

    DrawRectangleLinesEx(btnWeatherToggle, 2.0f, (currentWeather == WeatherState::SUNNY) ? mediumColor : wetColor);
    DrawTextEx(stratFont, (currentWeather == WeatherState::SUNNY) ? "WEATHER: SUNNY" : "WEATHER: RAINY", { btnWeatherToggle.x + 20, btnWeatherToggle.y + 15 }, 14, 2, WHITE);

    DrawTextEx(stratFont, "STARTING TYRE", { 720, 300 }, 20, 2, WHITE);
    
    DrawRectangleLinesEx(menuSoft, 2.0f, (startingTyre == TyreType::SOFT) ? softColor : borderGray);
    DrawTextEx(stratFont, "SOFT", { menuSoft.x + 20, menuSoft.y + 15 }, 14, 2, (startingTyre == TyreType::SOFT) ? softColor : textDim);
    
    DrawRectangleLinesEx(menuMed, 2.0f, (startingTyre == TyreType::MEDIUM) ? mediumColor : borderGray);
    DrawTextEx(stratFont, "MEDIUM", { menuMed.x + 20, menuMed.y + 15 }, 14, 2, (startingTyre == TyreType::MEDIUM) ? mediumColor : textDim);
    
    DrawRectangleLinesEx(menuHard, 2.0f, (startingTyre == TyreType::HARD) ? hardColor : borderGray);
    DrawTextEx(stratFont, "HARD", { menuHard.x + 20, menuHard.y + 15 }, 14, 2, (startingTyre == TyreType::HARD) ? hardColor : textDim);

    DrawRectangleLinesEx(menuWet, 2.0f, (startingTyre == TyreType::WET) ? wetColor : borderGray);
    DrawTextEx(stratFont, "WET", { menuWet.x + 20, menuWet.y + 12 }, 14, 2, (startingTyre == TyreType::WET) ? wetColor : textDim);

    bool startHover = CheckCollisionPointRec(mousePos, startRaceBtn);
    DrawRectangleRec(startRaceBtn, startHover ? WHITE : neonCyan);
    DrawTextEx(stratFont, "START SESSION", { startRaceBtn.x + 95, startRaceBtn.y + 20 }, 20, 2, BLACK);
}

void Game::UpdateRace() {
    if (gameState == 1) {
        startTimer++; 
        if (startTimer > lightsOutDelay) gameState = 2; 
        return; 
    }

    Vector2 mousePos = GetMousePosition();
    
    bool allFinished = true;
    for (int i = 0; i < 10; i++) {
        if (!cars[i].finished) allFinished = false;
    }

    if (!allFinished) {
        raceTime += GetFrameTime(); 
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, btnSoft)) nextTyre = TyreType::SOFT;
        if (CheckCollisionPointRec(mousePos, btnMedium)) nextTyre = TyreType::MEDIUM;
        if (CheckCollisionPointRec(mousePos, btnHard)) nextTyre = TyreType::HARD;
        if (CheckCollisionPointRec(mousePos, btnWet)) nextTyre = TyreType::WET;

        if (CheckCollisionPointRec(mousePos, boxButton) && cars[playerID].pitTimer == 0 && !cars[playerID].finished) {
            cars[playerID].pitRequested = true; 
        }
    }

    for (int i = 0; i < 10; i++) {
        if (i != playerID) {
            bool needsToPit = false;
            
            if (cars[i].tyreWear < 15.0f) needsToPit = true;
            
            if (currentWeather == WeatherState::RAINY && cars[i].tyre != TyreType::WET) {
                if (GetRandomValue(0, 5) == 0) needsToPit = true; 
            }
            if (currentWeather == WeatherState::SUNNY && cars[i].tyre == TyreType::WET) {
                if (GetRandomValue(0, 5) == 0) needsToPit = true;
            }

            if (needsToPit && cars[i].pitTimer == 0 && !cars[i].finished) {
                cars[i].pitRequested = true;
            }
        }

        float currentWearRate = (cars[i].tyre == TyreType::SOFT) ? 0.08f : (cars[i].tyre == TyreType::MEDIUM) ? 0.05f : 0.02f;
        float tyreSpeed = cars[i].speedBase;
        
        if (currentWeather == WeatherState::SUNNY) {
            if (cars[i].tyre == TyreType::SOFT) {
                tyreSpeed += 0.006f; 
                tyreSpeed -= (100.0f - cars[i].tyreWear) * 0.0001f;
            } else if (cars[i].tyre == TyreType::MEDIUM) {
                tyreSpeed += 0.001f; 
                tyreSpeed -= (100.0f - cars[i].tyreWear) * 0.00005f; 
            } else if (cars[i].tyre == TyreType::HARD) {
                tyreSpeed -= 0.001f; 
                tyreSpeed -= (100.0f - cars[i].tyreWear) * 0.00001f;
            } else if (cars[i].tyre == TyreType::WET) {
                tyreSpeed -= 0.010f; 
                currentWearRate = 0.20f;
            }
        } else if (currentWeather == WeatherState::RAINY) {
            if (cars[i].tyre == TyreType::WET) {
                tyreSpeed += 0.005f; 
                currentWearRate = 0.03f; 
                tyreSpeed -= (100.0f - cars[i].tyreWear) * 0.00005f;
            } else {
                tyreSpeed -= 0.015f; 
                currentWearRate = 0.01f; 
            }
        }
        
        if (i == playerID) tyreSpeed += 0.001f;
        if (tyreSpeed < 0.010f) tyreSpeed = 0.010f; 

        if (currentWeather == WeatherState::RAINY && cars[i].pitTimer == 0 && !cars[i].finished) {
            int spinChance = (cars[i].tyre == TyreType::WET) ? 3000 : 300; 

            if (GetRandomValue(0, spinChance) == 0 && cars[i].spinTimer == 0) {
                cars[i].spinTimer = GetRandomValue(40, 120); 
                cars[i].tyreWear -= 8.0f; 
                if (cars[i].tyreWear < 0.0f) {
                    cars[i].tyreWear = 0.0f; 
                }
            }
        }

        if (!cars[i].finished) {
            if (cars[i].spinTimer > 0) {
                cars[i].spinTimer--;
                cars[i].lineOffset += static_cast<float>(GetRandomValue(-2, 2)) / 2.0f;
                continue;
            }

            if (cars[i].pitTimer > 0) {
                cars[i].pitTimer--;
                if (cars[i].pitTimer == 0) {
                    cars[i].tyreWear = 100.0f; 
                    if (i == playerID) {
                        cars[i].tyre = nextTyre;
                    } else {
                        if (currentWeather == WeatherState::RAINY) {
                            cars[i].tyre = TyreType::WET;
                        } else {
                            cars[i].tyre = TyreType::HARD; 
                        }
                    }
                }
            } else {
                cars[i].currentAngle += tyreSpeed; 
                
                if (cars[i].currentAngle >= 2 * M_PI_F) {
                    cars[i].currentAngle -= 2 * M_PI_F;
                    cars[i].laps++;
                    
                    if (cars[i].laps >= totalLaps) {
                        cars[i].finished = true;
                        cars[i].finishTime = raceTime; 
                    } else if (cars[i].pitRequested) {
                        cars[i].pitTimer = 120;
                        cars[i].pitRequested = false;
                        cars[i].currentAngle = 0.0f; 
                    }
                }
                
                if (cars[i].pitTimer == 0) {
                    cars[i].position.x = trackCenter.x + cos(cars[i].currentAngle) * (trackRadiusX + cars[i].lineOffset);
                    cars[i].position.y = trackCenter.y + sin(cars[i].currentAngle) * (trackRadiusY + cars[i].lineOffset);
                    cars[i].tyreWear -= currentWearRate;
                    if (cars[i].tyreWear < 0) cars[i].tyreWear = 0.0f;
                }
            }
        }
    }

    SortStandings();

    if (allFinished) {
        postRaceTimer += GetFrameTime();
        if (postRaceTimer > 3.0f) {
            gameState = 3; 
        }
    }
}

void Game::DrawRace() {
    Vector2 mousePos = GetMousePosition();
    
    int playerPos = 1;
    for (int i = 0; i < 10; i++) {
        if (standings[i] == playerID) playerPos = i + 1;
    }

    DrawRectangleLines(20, 20, 300, 760, borderGray);
    DrawRectangle(20, 20, 300, 2, cars[playerID].teamColor);
    DrawTextEx(stratFont, "TEL_01 // LIVE DATA", { 35, 35 }, 14, 2, textDim);
    DrawTextEx(stratFont, cars[playerID].name.c_str(), { 40, 60 }, 20, 2, cars[playerID].teamColor);

    DrawTextEx(stratFont, TextFormat("POS: %d / 10", playerPos), { 40, 100 }, 26, 2, neonCyan);

    float displayTime = cars[playerID].finished ? cars[playerID].finishTime : raceTime;
    int minutes = static_cast<int>(displayTime) / 60;
    float seconds = displayTime - (minutes * 60);
    DrawTextEx(stratFont, TextFormat("TIME: %02d:%05.2f", minutes, seconds), { 40, 130 }, 20, 2, WHITE);

    if (cars[playerID].pitTimer > 0) {
        DrawTextEx(stratFont, "IN PIT", { 70, 180 }, 80, 2, neonRed);
    } else if (cars[playerID].finished) {
        DrawTextEx(stratFont, "FIN", { 110, 220 }, 100, 2, neonGreen);
    } else { 
        DrawTextEx(stratFont, "314", { 80, 180 }, 100, 2, neonCyan);
        DrawTextEx(stratFont, "KPH", { 230, 240 }, 24, 2, textDim);
    }
    
    DrawTextEx(stratFont, "TIRE WEAR:", { 40, 300 }, 20, 2, textDim);
    DrawRectangle(40, 330, 200, 10, borderGray);
    DrawRectangle(40, 330, static_cast<int>(cars[playerID].tyreWear * 2), 10, (cars[playerID].tyreWear > 30.0f) ? neonGreen : neonRed); 

    const char* currentTyreText = (cars[playerID].tyre == TyreType::SOFT) ? "SOFT" : (cars[playerID].tyre == TyreType::MEDIUM) ? "MEDIUM" : (cars[playerID].tyre == TyreType::HARD) ? "HARD" : "WET";
    Color currentTyreColor = (cars[playerID].tyre == TyreType::SOFT) ? softColor : (cars[playerID].tyre == TyreType::MEDIUM) ? mediumColor : (cars[playerID].tyre == TyreType::HARD) ? hardColor : wetColor;

    DrawTextEx(stratFont, "CURRENT TYRE:", { 40, 360 }, 14, 2, textDim);
    DrawTextEx(stratFont, currentTyreText, { 190, 360 }, 14, 2, currentTyreColor);

    DrawRectangleLines(340, 20, 600, 760, borderGray);
    DrawRectangle(340, 20, 600, 2, neonGreen);
    DrawTextEx(stratFont, "SECTOR MAP // SILVERSTONE", { 355, 35 }, 14, 2, textDim);
    DrawTextEx(stratFont, TextFormat("LAP %d / %d", cars[playerID].laps + 1 > totalLaps ? totalLaps : cars[playerID].laps + 1, totalLaps), { 570, 380 }, 30, 2, WHITE);
    DrawTextEx(stratFont, (currentWeather == WeatherState::SUNNY) ? "TRACK: DRY" : "TRACK: WET", { 570, 420 }, 20, 2, (currentWeather == WeatherState::SUNNY) ? mediumColor : wetColor);
    
    DrawEllipseLines(trackCenter.x, trackCenter.y, trackRadiusX, trackRadiusY, borderGray);
    DrawEllipseLines(trackCenter.x, trackCenter.y, trackRadiusX + 12, trackRadiusY + 12, borderGray);
    DrawEllipseLines(trackCenter.x, trackCenter.y, trackRadiusX + 24, trackRadiusY + 24, borderGray);

    DrawLineEx({trackCenter.x + trackRadiusX - 10, trackCenter.y}, 
               {trackCenter.x + trackRadiusX + 35, trackCenter.y}, 4, WHITE);
    DrawTextEx(stratFont, "START / PIT", {trackCenter.x + trackRadiusX - 30, trackCenter.y - 20}, 10, 2, textDim);

    if (gameState == 1) {
        int lightsOn = startTimer / 60; 
        if (lightsOn > 5) lightsOn = 5;
        
        for (int i = 0; i < 5; i++) {
            Color lightColor = (i < lightsOn) ? RED : ColorAlpha(DARKGRAY, 0.5f);
            DrawCircle(trackCenter.x - 100 + (i * 50), trackCenter.y - 100, 15, lightColor);
            DrawCircleLines(trackCenter.x - 100 + (i * 50), trackCenter.y - 100, 15, borderGray);
        }
    }

    for (int i = 0; i < 10; i++) {
        if (cars[i].pitTimer == 0 && !cars[i].finished) {
            DrawCircleV(cars[i].position, 6.0f, cars[i].teamColor);
            if (i == playerID) DrawRectangleLines(cars[i].position.x - 8, cars[i].position.y - 8, 16, 16, WHITE); 
        }
    }

    DrawRectangleLines(960, 20, 300, 760, borderGray);
    DrawRectangle(960, 20, 300, 2, neonRed);
    DrawTextEx(stratFont, "PIT_WALL // COMMANDS", { 975, 35 }, 14, 2, textDim);
    
    if (cars[playerID].pitRequested) { 
        DrawRectangleRec(boxButton, mediumColor);
        DrawTextEx(stratFont, "BOX CONFIRMED", { 1010, 95 }, 20, 2, BLACK);
    } else {
        Color btnColor = CheckCollisionPointRec(mousePos, boxButton) ? WHITE : neonCyan;
        DrawRectangleRec(boxButton, btnColor);
        DrawTextEx(stratFont, "BOX THIS LAP", { 1030, 95 }, 20, 2, BLACK);
    }

    DrawTextEx(stratFont, "NEXT TYRE:", { 980, 130 }, 14, 2, textDim);
    
    DrawRectangleLinesEx(btnSoft, 2.0f, (nextTyre == TyreType::SOFT) ? softColor : borderGray);
    DrawTextEx(stratFont, "SOFT", { btnSoft.x + 12, btnSoft.y + 8 }, 14, 2, softColor);
    
    DrawRectangleLinesEx(btnMedium, 2.0f, (nextTyre == TyreType::MEDIUM) ? mediumColor : borderGray);
    DrawTextEx(stratFont, "MED", { btnMedium.x + 15, btnMedium.y + 8 }, 14, 2, mediumColor);
    
    DrawRectangleLinesEx(btnHard, 2.0f, (nextTyre == TyreType::HARD) ? hardColor : borderGray);
    DrawTextEx(stratFont, "HARD", { btnHard.x + 11, btnHard.y + 8 }, 14, 2, hardColor);
    
    DrawRectangleLinesEx(btnWet, 2.0f, (nextTyre == TyreType::WET) ? wetColor : borderGray);
    DrawTextEx(stratFont, "WET", { btnWet.x + 16, btnWet.y + 8 }, 14, 2, wetColor);

    DrawTextEx(stratFont, "LIVE STANDINGS:", { 980, 250 }, 14, 2, textDim);
    for(int i = 0; i < 5; i++) { 
        int driverIdx = standings[i];
        Color drvColor = cars[driverIdx].teamColor;
        DrawTextEx(stratFont, TextFormat("P%d %s", i+1, cars[driverIdx].name.c_str()), { 980.0f, 280.0f + (i * 25) }, 16, 2, drvColor);
    }
}

void Game::UpdateEndScreen() {
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, btnPlayAgain)) gameState = 0; 
        if (CheckCollisionPointRec(mousePos, btnExit)) closeGame = true;  
    }
}

void Game::DrawEndScreen() {
    Vector2 mousePos = GetMousePosition();
    
    DrawTextEx(stratFont, "FINAL CLASSIFICATION", { 1280/2 - 200, 80 }, 40, 2, WHITE);
    DrawRectangleLines(1280/2 - 400, 150, 800, 450, borderGray);
    DrawRectangle(1280/2 - 400, 150, 800, 2, neonGreen);

    for(int i = 0; i < 10; i++) {
        int driverIdx = standings[i];
        Color drvColor = cars[driverIdx].teamColor;
        float posY = 180.0f + (i * 40);
        
        DrawTextEx(stratFont, TextFormat("P%d", i+1), { 1280.0f/2 - 350, posY }, 24, 2, (i==0) ? neonGreen : WHITE);
        DrawRectangle(1280/2 - 290, posY + 5, 15, 15, drvColor);
        DrawTextEx(stratFont, cars[driverIdx].name.c_str(), { 1280.0f/2 - 260, posY + 2 }, 20, 2, WHITE);
        
        if (cars[driverIdx].finished) {
            int m = static_cast<int>(cars[driverIdx].finishTime) / 60;
            float s = cars[driverIdx].finishTime - (m * 60);
            DrawTextEx(stratFont, TextFormat("%02d:%05.2f", m, s), { 1280.0f/2 + 200, posY + 2 }, 20, 2, textDim);
        } else {
            DrawTextEx(stratFont, "DNF", { 1280.0f/2 + 250, posY + 2 }, 20, 2, neonRed);
        }
    }

    bool playHover = CheckCollisionPointRec(mousePos, btnPlayAgain);
    DrawRectangleLinesEx(btnPlayAgain, 2.0f, playHover ? neonCyan : borderGray);
    DrawTextEx(stratFont, "PLAY AGAIN", { btnPlayAgain.x + 50, btnPlayAgain.y + 20 }, 20, 2, playHover ? neonCyan : WHITE);
    
    bool exitHover = CheckCollisionPointRec(mousePos, btnExit);
    DrawRectangleLinesEx(btnExit, 2.0f, exitHover ? neonRed : borderGray);
    DrawTextEx(stratFont, "EXIT", { btnExit.x + 80, btnExit.y + 20 }, 20, 2, exitHover ? neonRed : WHITE);
}