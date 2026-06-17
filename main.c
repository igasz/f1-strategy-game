#include "game.h"


int main(void)
{
    // inicjalizacja
    const int screenWidth = 1280;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "PITWALL_OS // RACE COMMAND");
    SetTargetFPS(60); 

    // tworzenie gry
    Game g = { 0 }; 
    
    InitGame(&g);

    // główna pętla
    while (!WindowShouldClose() && !g.closeGame) {
        
        // stany gry
        if (g.gameState == 0) {
            UpdateMenu(&g);
        } else if (g.gameState == 1 || g.gameState == 2) {
            UpdateRace(&g);
        } else if (g.gameState == 3) {
            UpdateEndScreen(&g);
        }

        // rysowanie
        BeginDrawing();
        ClearBackground(bgDark); // Czyścimy ekran
        
        if (g.gameState == 0) {
            DrawMenu(&g);
        } else if (g.gameState == 1 || g.gameState == 2) {
            DrawRace(&g);
        } else if (g.gameState == 3) {
            DrawEndScreen(&g);
        }
        
        EndDrawing();
    }

    // zamykanie
    UnloadFont(g.stratFont); 
    CloseWindow();
    return 0;
}