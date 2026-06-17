# F1 strategy game
### author: Iga Szaflik

Prosta gra strategiczna o tematyce Formuły 1, napisana w języku C++ z wykorzystaniem biblioteki **raylib**. Wciel się w rolę głównego stratega wyścigowego, analizuj telemetrię na żywo i podejmuj kluczowe decyzje ułamki sekund przed rywalami!

---

## Główne funkcjonalności

* **10 Zespołów:** Wybierz swoją ulubioną ekipę spośród prawdziwych zespołów stawki (Red Bull, Ferrari, Mercedes, McLaren i inne).
* **Zarządzanie oponami:** Dobierz optymalną mieszankę do warunków na torze:
    * 🔴 **SOFT** – Niesamowite tempo, ale drastyczna degradacja.
    * 🟡 **MEDIUM** – Idealny balans między prędkością a wytrzymałością.
    * ⚪ **HARD** – Wolniejsze, ale pozwalają na długie stinty.
    * 🔵 **WET** – Niezbędne, gdy nad torem zaczyna padać deszcz.
* **Dynamiczna Pogoda i AI:** Sytuacja na torze może ulec zmianie. Kiedy zaczyna padać, slicki stają się lodowiskiem. Rywale (AI) posiadają własną logikę zjazdów do boksu, ale popełniają też ludzkie błędy (np. uślizgi na mokrym torze).
* **Live Telemetry:** Śledź na bieżąco mapę toru (Silverstone), pozycje wszystkich kierowców, zużycie opon swojego bolidu oraz tabele z wynikami na żywo.

---

## Interfejs Gry

Gra jest podzielona na trzy główne ekrany:
1.  **Menu Startowe:** Wybór zespołu, opony startowej oraz warunków pogodowych.
2.  **Wyścig (Pit Wall):** Centrum dowodzenia z telemetrią, radarem i przyciskami decyzyjnymi.
3.  **Ekran Końcowy:** Ostateczna klasyfikacja wyścigu z wyliczonymi czasami.

---

## Technologie i Wymagania

Projekt został stworzony przy użyciu:
* **Język:** C++
* **Biblioteka graficzna:** [raylib](https://www.raylib.com/) (zoptymalizowana do tworzenia gier 2D/3D).

Aby skompilować grę samodzielnie, musisz posiadać zainstalowany kompilator C (np. MinGW/GCC) oraz skonfigurowane środowisko raylib. 
W przypadku Visual Studio Code z gotowym szablonem raylib, wystarczy uruchomić standardowy proces budowania (`Makefile`).

---

## Jak uruchomić projekt?

Projekt jest domyślnie przystosowany do oficjalnego środowiska **raylib** na systemie Windows (konfiguracja z edytorem Visual Studio Code i kompilatorem MinGW).

### Sposób 1: Przez Visual Studio Code (Zalecane)
1. Pobierz i zainstaluj oficjalny instalator środowiska ze strony raylib (wersja z wbudowanym kompilatorem dla Windows).
2. Sklonuj to repozytorium do folderu z projektami (domyślnie `C:\raylib\raylib\projects\VSCode`).
3. Otwórz ten folder w edytorze **Visual Studio Code**.
4. Wciśnij klawisz `F5` na klawiaturze – edytor automatycznie skompiluje pliki `game.c`, `main.c` i uruchomi grę.

### Sposób 2: Przez Terminal (przy użyciu Makefile)
Jeśli masz już skonfigurowany kompilator C (GCC/MinGW) i bibliotekę raylib:
1. Otwórz terminal w folderze z grą.
2. Wpisz komendę `mingw32-make` (lub `make`, w zależności od systemu), aby skompilować kod.
3. Uruchom wygenerowany plik `.exe` (na systemie Windows) dwukrotnym kliknięciem lub z poziomu terminala wpisując `./game.exe`.

##  Jak grać?

1.  Uruchom grę i w **Menu Startowym** kliknij na zespół, który chcesz poprowadzić.
2.  Wybierz oponę startową oraz ustaw pogodę. Kliknij **START SESSION**.
3.  Podczas wyścigu obserwuj pasek **TIRE WEAR** (Zużycie opon). Gdy degradacja jest zbyt duża lub zmieniają się warunki pogodowe, wybierz mieszankę z sekcji **NEXT TYRE** i wciśnij **BOX THIS LAP**.
4.  Dojeedź do mety na najwyższym możliwym miejscu, pokonując strategię rywali!

---
