#include "Game.hpp"

int main() {
    // Inicjalizacja biblioteki Raylib
    const int screenWidth = 1280;
    const int screenHeight = 800;
    
    // Okno musi być zainicjowane przed jakimkolwiek ładowaniem zasobów
    InitWindow(screenWidth, screenHeight, "PITWALL_OS // RACE COMMAND");
    SetTargetFPS(60); 

    // Ograniczamy zakres życia obiektu Game
    {
        Game game;
        game.Run();
    }

    // Zamykanie Raylib
    CloseWindow();
    
    return 0;
}