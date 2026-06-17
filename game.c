#include "game.h"

// kolory
const Color bgDark = { 18, 18, 20, 255 };
const Color neonCyan = { 0, 243, 255, 255 };
const Color neonGreen = { 183, 247, 0, 255 };
const Color neonRed = { 255, 180, 171, 255 };
const Color borderGray = { 34, 34, 38, 255 };
const Color textDim = { 185, 202, 203, 255 };
const Color softColor = { 255, 80, 80, 255 };
const Color mediumColor = { 255, 200, 0, 255 };
const Color hardColor = { 220, 220, 220, 255 };
const Color wetColor = { 0, 150, 255, 255 };

Color teamColors[10] = {
    { 54, 113, 198, 255 },  // 0. Red Bull Racing (Deep Blue)
    { 232, 0, 32, 255 },    // 1. Ferrari (Red)
    { 39, 244, 210, 255 },  // 2. Mercedes (Petronas Teal)
    { 255, 128, 0, 255 },   // 3. McLaren (Papaya Orange)
    { 34, 89, 61, 255 },    // 4. Aston Martin (British Racing Green)
    { 255, 135, 188, 255 }, // 5. Alpine (BWT Pink)
    { 0, 160, 222, 255 },   // 6. Williams (Light Blue)
    { 102, 146, 255, 255 }, // 7. VCARB (Blue/Silver)
    { 82, 226, 82, 255 },   // 8. Kick Sauber (Neon Green)
    { 255, 255, 255, 255 }  // 9. Haas (White/Red)
};

const char* teamNames[10] = {
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

// funkcje

void InitGame(Game* g) {
    g->stratFont = LoadFontEx("Lato-Regular.ttf", 120, 0, 0);
    g->totalLaps = 10;
    g->trackCenter = (Vector2){ 640, 400 };
    g->trackRadiusX = 220.0f;
    g->trackRadiusY = 320.0f;
    
    g->gameState = 0; // menu
    g->playerID = 0;
    g->startingTyre = SOFT;
    g->nextTyre = SOFT;
    g->closeGame = false;

    // przyciski
    for (int i = 0; i < 10; i++) {
        g->teamBtns[i] = (Rectangle){ 150, 220 + (i * 45), 450, 40 };
    }

    g->currentWeather = SUNNY; // Słoneczna pogoda na start

    g->btnWeatherToggle = (Rectangle){ 720, 242, 370, 45 };

    g->menuSoft = (Rectangle){ 720, 340, 370, 40 };
    g->menuMed  = (Rectangle){ 720, 390, 370, 40 };
    g->menuHard = (Rectangle){ 720, 440, 370, 40 };
    g->menuWet  = (Rectangle){ 720, 490, 370, 40 };
    
    g->startRaceBtn = (Rectangle){ 720, 560, 370, 60 };

    g->boxButton = (Rectangle){ 980, 80, 260, 50 };
    
    g->btnSoft   = (Rectangle){ 970, 150, 60, 30 };
    g->btnMedium = (Rectangle){ 1040, 150, 60, 30 };
    g->btnHard   = (Rectangle){ 1110, 150, 60, 30 };
    g->btnWet    = (Rectangle){ 1180, 150, 60, 30 };
    
    g->btnPlayAgain = (Rectangle){ 1280/2 - 280, 650, 260, 60 };
    g->btnExit      = (Rectangle){ 1280/2 + 20, 650, 260, 60 };
}

// Funkcja przygotowująca bolidy do jazdy
void StartSession(Game* g) {
    for (int i = 0; i < 10; i++) {
        g->cars[i].teamColor = teamColors[i];
        g->cars[i].name = teamNames[i];
        
        // losowanie opon
        if (i == g->playerID) {
            g->cars[i].tyre = g->startingTyre;
        } else {
            if (g->currentWeather == RAINY) {
                g->cars[i].tyre = WET; // Gdy leje od początku, bierze deszczówki
            } else {
                g->cars[i].tyre = (TyreType)GetRandomValue(0, 2);
            }
        }
        
        // rozstawiamy
        g->cars[i].currentAngle = -(i * 0.08f); 
        g->cars[i].speedBase = 0.021f + ((float)GetRandomValue(-10, 10) / 10000.0f);
        
        g->cars[i].lineOffset = (float)(i % 3) * 12.0f; 
        g->cars[i].position.x = g->trackCenter.x + cos(g->cars[i].currentAngle) * (g->trackRadiusX + g->cars[i].lineOffset);
        g->cars[i].position.y = g->trackCenter.y + sin(g->cars[i].currentAngle) * (g->trackRadiusY + g->cars[i].lineOffset);
        
        g->cars[i].laps = 0;
        g->cars[i].pitTimer = 0;
        g->cars[i].finished = false;
        g->cars[i].pitRequested = false;
        g->cars[i].finishTime = 0.0f;
        g->cars[i].spinTimer = 0;
        
        g->standings[i] = i; // reset tabeli
    }
    
    g->nextTyre = g->startingTyre; 
    g->raceTime = 0.0f;
    g->postRaceTimer = 0.0f;
    
    // stan gry = 1 (odliczanie świateł)
    g->gameState = 1;
    g->startTimer = 0;
    g->lightsOutDelay = 240 + GetRandomValue(30, 120); // Losowy czas zgaszenia czerwonych świateł
}

// bubble sort - Porównuje auta parami i lepszy do góry tabeli.
void SortStandings(Game* g) {
    for(int j = 0; j < 9; j++) {
        for(int k = 0; k < 9 - j; k++) {
             
            // Jeśli skończył: (100 000 pkt - czas wyścigu). Im szybszy czas, tym wyższy wynik.
            // Jeśli jedzie: Okrążenia pomnożone przez 1000 + aktualny kąt skrętu na torze.
            int indexA = g->standings[k];       // wyżej w tabeli
            int indexB = g->standings[k+1];     // niżej

            Bolid carA = g->cars[indexA];
            Bolid carB = g->cars[indexB];

            float scoreA;
            float scoreB;

            if (carA.finished == true) {
                scoreA = 100000.0f - carA.finishTime;
            } 
            else {
                scoreA = (carA.laps * 1000.0f) + carA.currentAngle;
            }

            if (carB.finished == true) {
                scoreB = 100000.0f - carB.finishTime;
            } 
            else {
                scoreB = (carB.laps * 1000.0f) + carB.currentAngle;
            }

            // Zamiana miejsc
            if (scoreB > scoreA) {
                int temp = g->standings[k];
                g->standings[k] = g->standings[k+1];
                g->standings[k+1] = temp;
            }
        }
    }
}

// menu stan = 0
void UpdateMenu(Game* g) {
    Vector2 mousePos = GetMousePosition();

    // Wykrywanie kliknięcia w zespół
    for (int i = 0; i < 10; i++) {
        if (CheckCollisionPointRec(mousePos, g->teamBtns[i]) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            g->playerID = i;
        }
    }

    // Wybór opony na start
    if (CheckCollisionPointRec(mousePos, g->menuSoft) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        g->startingTyre = SOFT;
    }

    if (CheckCollisionPointRec(mousePos, g->menuMed) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        g->startingTyre = MEDIUM;
    }
    
    if (CheckCollisionPointRec(mousePos, g->menuHard) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        g->startingTyre = HARD;
    }

    if (CheckCollisionPointRec(mousePos, g->menuWet) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        g->startingTyre = WET;
    }

    // pogoda
    if (CheckCollisionPointRec(mousePos, g->btnWeatherToggle) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        g->currentWeather = (g->currentWeather == SUNNY) ? RAINY : SUNNY;
    }

    // Przycisk Start
    if (CheckCollisionPointRec(mousePos, g->startRaceBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        StartSession(g);
    }
}

void DrawMenu(Game* g) {
    Vector2 mousePos = GetMousePosition();
    
    DrawTextEx(g->stratFont, "INITIALIZATION", (Vector2){ 150, 100 }, 40, 2, WHITE);
    DrawTextEx(g->stratFont, "CHOOSE YOUR TEAM AND STARTING STRATEGY", (Vector2){ 150, 150 }, 14, 2, textDim);

    // Renderowanie 10 przycisków zespołów z podświetleniami
    for (int i = 0; i < 10; i++) {
        bool isHovered = CheckCollisionPointRec(mousePos, g->teamBtns[i]);
        Color btnBorder = isHovered ? WHITE : borderGray;
        if (g->playerID == i) btnBorder = neonCyan;
        
        DrawRectangleLinesEx(g->teamBtns[i], (g->playerID == i) ? 3 : 2, btnBorder);
        DrawRectangle(g->teamBtns[i].x + 15, g->teamBtns[i].y + 10, 20, 20, teamColors[i]);
        DrawTextEx(g->stratFont, teamNames[i], (Vector2){ g->teamBtns[i].x + 55, g->teamBtns[i].y + 12 }, 20, 2, (g->playerID == i || isHovered) ? WHITE : textDim);
    }

    // Prawy interfejs opon
    DrawRectangleLines(680, 220, 450, 450, borderGray);
    DrawRectangle(680, 220, 450, 2, neonCyan);

    // Przycisk pogody
    DrawRectangleLinesEx(g->btnWeatherToggle, 2, (g->currentWeather == SUNNY) ? mediumColor : wetColor);
    DrawTextEx(g->stratFont, (g->currentWeather == SUNNY) ? "WEATHER: SUNNY" : "WEATHER: RAINY", (Vector2){ g->btnWeatherToggle.x + 20, g->btnWeatherToggle.y + 15 }, 14, 2, WHITE);

    // opony    
    DrawTextEx(g->stratFont, "STARTING TYRE", (Vector2){ 720, 300 }, 20, 2, WHITE);
    
    DrawRectangleLinesEx(g->menuSoft, 2, (g->startingTyre == SOFT) ? softColor : borderGray);
    DrawTextEx(g->stratFont, "SOFT", (Vector2){ g->menuSoft.x + 20, g->menuSoft.y + 15 }, 14, 2, (g->startingTyre == SOFT) ? softColor : textDim);
    
    DrawRectangleLinesEx(g->menuMed, 2, (g->startingTyre == MEDIUM) ? mediumColor : borderGray);
    DrawTextEx(g->stratFont, "MEDIUM", (Vector2){ g->menuMed.x + 20, g->menuMed.y + 15 }, 14, 2, (g->startingTyre == MEDIUM) ? mediumColor : textDim);
    
    DrawRectangleLinesEx(g->menuHard, 2, (g->startingTyre == HARD) ? hardColor : borderGray);
    DrawTextEx(g->stratFont, "HARD", (Vector2){ g->menuHard.x + 20, g->menuHard.y + 15 }, 14, 2, (g->startingTyre == HARD) ? hardColor : textDim);

    DrawRectangleLinesEx(g->menuWet, 2, (g->startingTyre == WET) ? wetColor : borderGray);
    DrawTextEx(g->stratFont, "WET", (Vector2){ g->menuWet.x + 20, g->menuWet.y + 12 }, 14, 2, (g->startingTyre == WET) ? wetColor : textDim);

    // Przycisk Start
    bool startHover = CheckCollisionPointRec(mousePos, g->startRaceBtn);
    DrawRectangleRec(g->startRaceBtn, startHover ? WHITE : neonCyan);
    DrawTextEx(g->stratFont, "START SESSION", (Vector2){ g->startRaceBtn.x + 95, g->startRaceBtn.y + 20 }, 20, 2, BLACK);
}

// wyscig stan 1 i 2
void UpdateRace(Game* g) {
    
    // odliczanie
    if (g->gameState == 1) {
        g->startTimer++; 
        if (g->startTimer > g->lightsOutDelay) g->gameState = 2; // światła zgasły
        return; 
    }

    Vector2 mousePos = GetMousePosition();
    
    // Zabezpieczenie przed przedwczesnym wyłączeniem zegara
    bool allFinished = true;
    for (int i = 0; i < 10; i++) {
        if (!g->cars[i].finished) allFinished = false;
    }

    // stoper odlicza do ostatniego
    if (!allFinished) {
        g->raceTime += GetFrameTime(); 
    }

    // Pit Wall
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, g->btnSoft)) g->nextTyre = SOFT;
        if (CheckCollisionPointRec(mousePos, g->btnMedium)) g->nextTyre = MEDIUM;
        if (CheckCollisionPointRec(mousePos, g->btnHard)) g->nextTyre = HARD;
        if (CheckCollisionPointRec(mousePos, g->btnWet)) g->nextTyre = WET;

        if (CheckCollisionPointRec(mousePos, g->boxButton) && g->cars[g->playerID].pitTimer == 0 && !g->cars[g->playerID].finished) {
            g->cars[g->playerID].pitRequested = true; 
        }
    }


    for (int i = 0; i < 10; i++) {
        
        // rywale
        if (i != g->playerID) {
            bool needsToPit = false;
            
            // Zużycie opon
            if (g->cars[i].tyreWear < 15.0f) needsToPit = true;
            
            // Reakcja na pogodę
           if (g->currentWeather == RAINY && g->cars[i].tyre != WET) {
                if (GetRandomValue(0, 5) == 0) needsToPit = true; 
            }
            if (g->currentWeather == SUNNY && g->cars[i].tyre == WET) {
                if (GetRandomValue(0, 5) == 0) needsToPit = true;
            }

            if (needsToPit && g->cars[i].pitTimer == 0 && !g->cars[i].finished) {
                g->cars[i].pitRequested = true;
            }

            if (needsToPit && g->cars[i].pitTimer == 0 && !g->cars[i].finished) {
                g->cars[i].pitRequested = true;
            }
        }

        // opony
        float currentWearRate = (g->cars[i].tyre == SOFT) ? 0.08f : (g->cars[i].tyre == MEDIUM) ? 0.05f : 0.02f;
        float tyreSpeed = g->cars[i].speedBase;
        
        
        // sucho
        if (g->currentWeather == SUNNY) {
            if (g->cars[i].tyre == SOFT) {
                tyreSpeed += 0.006f; // szybkie
                tyreSpeed -= (100.0f - g->cars[i].tyreWear) * 0.0001f;
            } else if (g->cars[i].tyre == MEDIUM) {
                tyreSpeed += 0.001f; // Neutralne
                tyreSpeed -= (100.0f - g->cars[i].tyreWear) * 0.00005f; 
            } else if (g->cars[i].tyre == HARD) {
                tyreSpeed -= 0.001f; // wolne
                tyreSpeed -= (100.0f - g->cars[i].tyreWear) * 0.00001f;
            } else if (g->cars[i].tyre == WET) {
                // wolne i szybko się niszczą
                tyreSpeed -= 0.010f; 
                currentWearRate = 0.20f;
            }
        } 
        // mokro
        else if (g->currentWeather == RAINY) {
            if (g->cars[i].tyre == WET) {
                tyreSpeed += 0.005f; // najszybsze
                currentWearRate = 0.03f; // niskie zużycie, woda chłodzi
                tyreSpeed -= (100.0f - g->cars[i].tyreWear) * 0.00005f;
            } else {
                // inne wolne
                tyreSpeed -= 0.015f; 
                currentWearRate = 0.01f; 
            }
        }
        
        if (i == g->playerID) tyreSpeed += 0.001f;
        if (tyreSpeed < 0.010f) tyreSpeed = 0.010f; 

        // spin
        if (g->currentWeather == RAINY && g->cars[i].pitTimer == 0 && !g->cars[i].finished) {
            int spinChance = (g->cars[i].tyre == WET) ? 3000 : 300; 

            if (GetRandomValue(0, spinChance) == 0 && g->cars[i].spinTimer == 0) {
                g->cars[i].spinTimer = GetRandomValue(40, 120); // Stoi w miejscu od 0.5s do 2s (kara)
            }
        }

        // ruch
        if (!g->cars[i].finished) {
            // poślizg
            if (g->cars[i].spinTimer > 0) {
                g->cars[i].spinTimer--;
                // lekkie trzęsienie
                g->cars[i].lineOffset += (float)GetRandomValue(-2, 2) / 2.0f;
                continue;
            }

            if (g->cars[i].pitTimer > 0) {
                // pit
                g->cars[i].pitTimer--;
                if (g->cars[i].pitTimer == 0) {
                    g->cars[i].tyreWear = 100.0f; // nowe opony
                    if (i == g->playerID) {
                        g->cars[i].tyre = g->nextTyre;
                    } else {
                        if (g->currentWeather == RAINY) {
                            g->cars[i].tyre = WET;
                        }
                        else {
                            g->cars[i].tyre = HARD; // rywale 2 stop hardy
                        }
                    }
                    
                }
            } else {
                g->cars[i].currentAngle += tyreSpeed; 
                
                if (g->cars[i].currentAngle >= 2 * PI) {
                    g->cars[i].currentAngle -= 2 * PI;
                    g->cars[i].laps++;
                    
                    if (g->cars[i].laps >= g->totalLaps) {
                        g->cars[i].finished = true;
                        g->cars[i].finishTime = g->raceTime; 
                    } else if (g->cars[i].pitRequested) {
                        // pit
                        g->cars[i].pitTimer = 120;
                        g->cars[i].pitRequested = false;
                        g->cars[i].currentAngle = 0.0f; 
                    }
                }
                
                if (g->cars[i].pitTimer == 0) {
                    g->cars[i].position.x = g->trackCenter.x + cos(g->cars[i].currentAngle) * (g->trackRadiusX + g->cars[i].lineOffset);
                    g->cars[i].position.y = g->trackCenter.y + sin(g->cars[i].currentAngle) * (g->trackRadiusY + g->cars[i].lineOffset);
                    g->cars[i].tyreWear -= currentWearRate;
                    if (g->cars[i].tyreWear < 0) g->cars[i].tyreWear = 0.0f;
                }
            }
        }
    }

    SortStandings(g);

    // Kiedy ostatni dojedzie - ekran końcowy
    if (allFinished) {
        g->postRaceTimer += GetFrameTime();
        if (g->postRaceTimer > 3.0f) {
            g->gameState = 3; 
        }
    }
}

void DrawRace(Game* g) {
    Vector2 mousePos = GetMousePosition();
    
    // Szukamy, które miejsce zajmuje nasz kierowca do dużego tekstu POS
    int playerPos = 1;
    for (int i = 0; i < 10; i++) {
        if (g->standings[i] == g->playerID) playerPos = i + 1;
    }

    // telemetria
    DrawRectangleLines(20, 20, 300, 760, borderGray);
    DrawRectangle(20, 20, 300, 2, g->cars[g->playerID].teamColor);
    DrawTextEx(g->stratFont, "TEL_01 // LIVE DATA", (Vector2){ 35, 35 }, 14, 2, textDim);
    DrawTextEx(g->stratFont, g->cars[g->playerID].name, (Vector2){ 40, 60 }, 20, 2, g->cars[g->playerID].teamColor);

    DrawTextEx(g->stratFont, TextFormat("POS: %d / 10", playerPos), (Vector2){ 40, 100 }, 26, 2, neonCyan);

    // Wyświetlanie Czasu
    float displayTime = g->cars[g->playerID].finished ? g->cars[g->playerID].finishTime : g->raceTime;
    int minutes = (int)displayTime / 60;
    float seconds = displayTime - (minutes * 60);
    DrawTextEx(g->stratFont, TextFormat("TIME: %02d:%05.2f", minutes, seconds), (Vector2){ 40, 130 }, 20, 2, WHITE);

    if (g->cars[g->playerID].pitTimer > 0) {
        DrawTextEx(g->stratFont, "IN PIT", (Vector2){ 70, 180 }, 80, 2, neonRed);
    }

    else if (g->cars[g->playerID].finished) {
        DrawTextEx(g->stratFont, "FIN", (Vector2){ 110, 220 }, 100, 2, neonGreen);
    }

    else { 
        DrawTextEx(g->stratFont, "314", (Vector2){ 80, 180 }, 100, 2, neonCyan);
        DrawTextEx(g->stratFont, "KPH", (Vector2){ 230, 240 }, 24, 2, textDim);
    }
    
    DrawTextEx(g->stratFont, "TIRE WEAR:", (Vector2){ 40, 300 }, 20, 2, textDim);
    DrawRectangle(40, 330, 200, 10, borderGray);
    DrawRectangle(40, 330, (int)(g->cars[g->playerID].tyreWear * 2), 10, (g->cars[g->playerID].tyreWear > 30.0f) ? neonGreen : neonRed); 

    const char* currentTyreText = (g->cars[g->playerID].tyre == SOFT) ? "SOFT" : (g->cars[g->playerID].tyre == MEDIUM) ? "MEDIUM" : (g->cars[g->playerID].tyre == HARD) ? "HARD" : "WET";
    Color currentTyreColor = (g->cars[g->playerID].tyre == SOFT) ? softColor : (g->cars[g->playerID].tyre == MEDIUM) ? mediumColor : (g->cars[g->playerID].tyre == HARD) ? hardColor : wetColor;

    DrawTextEx(g->stratFont, "CURRENT TYRE:", (Vector2){ 40, 360 }, 14, 2, textDim);
    DrawTextEx(g->stratFont, currentTyreText, (Vector2){ 190, 360 }, 14, 2, currentTyreColor);

    // mapa
    DrawRectangleLines(340, 20, 600, 760, borderGray);
    DrawRectangle(340, 20, 600, 2, neonGreen);
    DrawTextEx(g->stratFont, "SECTOR MAP // SILVERSTONE", (Vector2){ 355, 35 }, 14, 2, textDim);
    DrawTextEx(g->stratFont, TextFormat("LAP %d / %d", g->cars[g->playerID].laps + 1 > g->totalLaps ? g->totalLaps : g->cars[g->playerID].laps + 1, g->totalLaps), (Vector2){ 570, 380 }, 30, 2, WHITE);
    DrawTextEx(g->stratFont, (g->currentWeather == SUNNY) ? "TRACK: DRY" : "TRACK: WET", (Vector2){ 570, 420 }, 20, 2, (g->currentWeather == SUNNY) ? mediumColor : wetColor);
    
    // Trzy linie wyścigowe
    DrawEllipseLines(g->trackCenter.x, g->trackCenter.y, g->trackRadiusX, g->trackRadiusY, borderGray);
    DrawEllipseLines(g->trackCenter.x, g->trackCenter.y, g->trackRadiusX + 12, g->trackRadiusY + 12, borderGray);
    DrawEllipseLines(g->trackCenter.x, g->trackCenter.y, g->trackRadiusX + 24, g->trackRadiusY + 24, borderGray);

    DrawLineEx((Vector2){g->trackCenter.x + g->trackRadiusX - 10, g->trackCenter.y}, 
               (Vector2){g->trackCenter.x + g->trackRadiusX + 35, g->trackCenter.y}, 4, WHITE);
    DrawTextEx(g->stratFont, "START / PIT", (Vector2){g->trackCenter.x + g->trackRadiusX - 30, g->trackCenter.y - 20}, 10, 2, textDim);

    // czerwone świarła
    if (g->gameState == 1) {
        int lightsOn = g->startTimer / 60; 
        
        if (lightsOn > 5) {
            lightsOn = 5;
        }
        
        for (int i = 0; i < 5; i++) {
            Color lightColor = (i < lightsOn) ? RED : ColorAlpha(DARKGRAY, 0.5f);
            DrawCircle(g->trackCenter.x - 100 + (i * 50), g->trackCenter.y - 100, 15, lightColor);
            DrawCircleLines(g->trackCenter.x - 100 + (i * 50), g->trackCenter.y - 100, 15, borderGray);
        }
    }

    // Renderowanie Bolidów
    for (int i = 0; i < 10; i++) {
        if (g->cars[i].pitTimer == 0 && !g->cars[i].finished) {
            DrawCircleV(g->cars[i].position, 6.0f, g->cars[i].teamColor);
            if (i == g->playerID) DrawRectangleLines(g->cars[i].position.x - 8, g->cars[i].position.y - 8, 16, 16, WHITE); // Celownik namierzający gracza
        }
    }

    // strategia, pit wall
    DrawRectangleLines(960, 20, 300, 760, borderGray);
    DrawRectangle(960, 20, 300, 2, neonRed);
    DrawTextEx(g->stratFont, "PIT_WALL // COMMANDS", (Vector2){ 975, 35 }, 14, 2, textDim);
    
    // Przycisk "Box" 
    if (g->cars[g->playerID].pitRequested) { 
        DrawRectangleRec(g->boxButton, mediumColor);
        DrawTextEx(g->stratFont, "BOX CONFIRMED", (Vector2){ 1010, 95 }, 20, 2, BLACK);
    } else {
        Color btnColor = CheckCollisionPointRec(mousePos, g->boxButton) ? WHITE : neonCyan;
        DrawRectangleRec(g->boxButton, btnColor);
        DrawTextEx(g->stratFont, "BOX THIS LAP", (Vector2){ 1030, 95 }, 20, 2, BLACK);
    }

    DrawTextEx(g->stratFont, "NEXT TYRE:", (Vector2){ 980, 130 }, 14, 2, textDim);
    
    DrawRectangleLinesEx(g->btnSoft, 2, (g->nextTyre == SOFT) ? softColor : borderGray);
    DrawTextEx(g->stratFont, "SOFT", (Vector2){ g->btnSoft.x + 12, g->btnSoft.y + 8 }, 14, 2, softColor);
    
    DrawRectangleLinesEx(g->btnMedium, 2, (g->nextTyre == MEDIUM) ? mediumColor : borderGray);
    DrawTextEx(g->stratFont, "MED", (Vector2){ g->btnMedium.x + 15, g->btnMedium.y + 8 }, 14, 2, mediumColor);
    
    DrawRectangleLinesEx(g->btnHard, 2, (g->nextTyre == HARD) ? hardColor : borderGray);
    DrawTextEx(g->stratFont, "HARD", (Vector2){ g->btnHard.x + 11, g->btnHard.y + 8 }, 14, 2, hardColor);
    
    DrawRectangleLinesEx(g->btnWet, 2, (g->nextTyre == WET) ? wetColor : borderGray);
    DrawTextEx(g->stratFont, "WET", (Vector2){ g->btnWet.x + 16, g->btnWet.y + 8 }, 14, 2, wetColor);

    

    DrawTextEx(g->stratFont, "LIVE STANDINGS:", (Vector2){ 980, 250 }, 14, 2, textDim);
    for(int i = 0; i < 5; i++) { 
        int driverIdx = g->standings[i];
        Color drvColor = g->cars[driverIdx].teamColor;
        DrawTextEx(g->stratFont, TextFormat("P%d %s", i+1, g->cars[driverIdx].name), (Vector2){ 980, 280 + (i * 25) }, 16, 2, drvColor);
    }
}

// zakończenie stan = 3
void UpdateEndScreen(Game* g) {
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, g->btnPlayAgain)) g->gameState = 0; // Wracamy do menu!
        if (CheckCollisionPointRec(mousePos, g->btnExit)) g->closeGame = true;   // System się zamknie.
    }
}

void DrawEndScreen(Game* g) {
    Vector2 mousePos = GetMousePosition();
    
    DrawTextEx(g->stratFont, "FINAL CLASSIFICATION", (Vector2){ 1280/2 - 200, 80 }, 40, 2, WHITE);
    DrawRectangleLines(1280/2 - 400, 150, 800, 450, borderGray);
    DrawRectangle(1280/2 - 400, 150, 800, 2, neonGreen);

    // Przelatujemy przez 10 pozycji
    for(int i = 0; i < 10; i++) {
        int driverIdx = g->standings[i];
        Color drvColor = g->cars[driverIdx].teamColor;
        int posY = 180 + (i * 40);
        
        DrawTextEx(g->stratFont, TextFormat("P%d", i+1), (Vector2){ 1280/2 - 350, posY }, 24, 2, (i==0) ? neonGreen : WHITE);
        DrawRectangle(1280/2 - 290, posY + 5, 15, 15, drvColor);
        DrawTextEx(g->stratFont, g->cars[driverIdx].name, (Vector2){ 1280/2 - 260, posY + 2 }, 20, 2, WHITE);
        
        // Zwrócony warunek DNF
        if (g->cars[driverIdx].finished) {
            int m = (int)g->cars[driverIdx].finishTime / 60;
            float s = g->cars[driverIdx].finishTime - (m * 60);
            DrawTextEx(g->stratFont, TextFormat("%02d:%05.2f", m, s), (Vector2){ 1280/2 + 200, posY + 2 }, 20, 2, textDim);
        } else {
            DrawTextEx(g->stratFont, "DNF", (Vector2){ 1280/2 + 250, posY + 2 }, 20, 2, neonRed);
        }
    }

    bool playHover = CheckCollisionPointRec(mousePos, g->btnPlayAgain);
    DrawRectangleLinesEx(g->btnPlayAgain, 2, playHover ? neonCyan : borderGray);
    DrawTextEx(g->stratFont, "PLAY AGAIN", (Vector2){ g->btnPlayAgain.x + 50, g->btnPlayAgain.y + 20 }, 20, 2, playHover ? neonCyan : WHITE);
    
    bool exitHover = CheckCollisionPointRec(mousePos, g->btnExit);
    DrawRectangleLinesEx(g->btnExit, 2, exitHover ? neonRed : borderGray);
    DrawTextEx(g->stratFont, "EXIT", (Vector2){ g->btnExit.x + 80, g->btnExit.y + 20 }, 20, 2, exitHover ? neonRed : WHITE);
}