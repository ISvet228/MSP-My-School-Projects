# Pong — Konzolna Igra (C++ / Windows Console)

Dokumentacija generisana isključivo na osnovu priloženog izvornog koda (`.cpp`, Windows Console API). Ne sadrži pretpostavke — samo ono što se stvarno nalazi u kodu.

---

## 1. Zaglavlja i Konfiguracija

```cpp
#include <windows.h>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
```

| Simbol | Vrednost / Tip | Opis |
|---|---|---|
| `WIDTH` | `int`, početno `80` | Širina konzolnog polja (menja se dinamički prema veličini prozora) |
| `HEIGHT` | `int`, početno `25` | Visina konzolnog polja (menja se dinamički prema veličini prozora) |
| `WIN_SCORE` | `const int = 10` | Broj poena potrebnih za pobedu |
| `PADDLE_H` | `const int = 4` | Visina reketa (palice) u karakterima |

---

## 2. Enum i Strukture

### `enum class State`
```cpp
Menu, TwoPlayers, AI, GameOver, Exit
```
Predstavlja trenutno stanje igre (mašina stanja).

### `struct Paddle`
```cpp
float y;
```
Vertikalna pozicija reketa.

### `struct Ball`
```cpp
float x, y, vx, vy;
```
Pozicija lopte (`x`, `y`) i njena brzina po osama (`vx`, `vy`).

---

## 3. Globalne Promenljive

| Promenljiva | Tip | Opis |
|---|---|---|
| `hConsole` | `HANDLE` | Handle na konzolni bafer za iscrtavanje |
| `screen` | `vector<char>` | Bafer karaktera koji se ispisuje u konzolu (veličine `WIDTH * HEIGHT`) |
| `state` | `State` | Trenutno stanje igre, počinje na `Menu` |
| `menuIndex` | `int` | Indeks trenutno selektovane stavke menija (0–2) |
| `leftPad`, `rightPad` | `Paddle` | Levi i desni reket |
| `ball` | `Ball` | Lopta |
| `leftScore`, `rightScore` | `int` | Trenutni rezultat obe strane |
| `aiMode` | `bool` | Da li se igra protiv AI (`true`) ili protiv drugog igrača (`false`) |
| `winnerText` | `string` | Tekst koji se prikazuje na ekranu kraja igre |
| `aiSpeed` | `float`, počinje `0.15f` | Brzina reagovanja AI reketa; raste tokom partije |

---

## 4. Funkcije za Iscrtavanje

| Funkcija | Šta radi |
|---|---|
| `ClearBuffer()` | Puni ceo `screen` bafer razmacima (`' '`) |
| `Put(x, y, c)` | Upisuje jedan karakter `c` na poziciju `(x, y)` u baferu, uz proveru granica |
| `DrawText(x, y, s)` | Ispisuje string `s` karakter po karakter počevši od `(x, y)` pozivajući `Put` |
| `Present()` | Šalje sadržaj `screen` bafera u konzolu preko `WriteConsoleOutputCharacterA` |
| `DrawBorders()` | Crta gornju i donju ivicu (`#`) i srednju liniju (`|`) polja |
| `DrawGame()` | Poziva `DrawBorders`, crta oba reketa, loptu (`O`) i ispisuje rezultat oba igrača |

---

## 5. Funkcije za Logiku Igre

### `UpdateConsoleSize()`
Čita stvarnu veličinu konzolnog prozora (`GetConsoleScreenBufferInfo`). Ako se promenila, ažurira `WIDTH`/`HEIGHT`, realocira `screen`, ograničava pozicije reketa unutar novih granica i, ako je lopta van polja, poziva `ResetBall()`.

### `ResetBall()`
Postavlja loptu na sredinu polja i daje joj nasumičan pravac:
- `vx` = `+0.45f` ili `-0.45f` (nasumično)
- `vy` = nasumična vrednost u opsegu otprilike `[-0.33f, 0.33f]`

### `StartGame(vsAI)`
Resetuje rezultate na 0, centrira oba reketa, resetuje `aiSpeed` na `0.15f`, poziva `ResetBall()` i postavlja `state` na `AI` ili `TwoPlayers` u zavisnosti od parametra `vsAI`.

### `KeyPressed(vk)`
Detektuje **jedan pritisak** tastera (a ne držanje) koristeći `GetAsyncKeyState` i statički niz prethodnih stanja po virtuelnom kodu tastera (`prev[256]`).

### `ScorePoint(leftPlayer)`
Uvećava rezultat odgovarajuće strane. Ako je `aiMode` aktivan, uvećava `aiSpeed` za `0.02f` (igra postaje teža). Ako neko dostigne `WIN_SCORE`, postavlja `winnerText` (tekst zavisi od toga da li je AI mod uključen) i prebacuje `state` na `GameOver`. Na kraju uvek poziva `ResetBall()`.

### `UpdateGame()`
Glavna fizika i logika po frejmu:
- Levi reket: `W`/`S` (drži se, ne "single press")
- Desni reket: strelice gore/dole — **samo ako `aiMode == false`**
- Ako je `aiMode == true`, AI pomera `rightPad` prema centru lopte brzinom `aiSpeed` (jednostavno prati vertikalnu poziciju lopte)
- Ograničava oba reketa unutar granica polja
- Pomera loptu (`ball.x += vx`, `ball.y += vy`)
- Odbija loptu od gornje/donje ivice (`vy = -vy`)
- Detektuje odboj od reketa (na fiksnim x-pozicijama `3` i `WIDTH - 4`): ako lopta pogodi reket, `vx` menja znak i **ubrzava se za 5%** (`* 1.05f`), uz zvučni signal `Beep(900, 10)`
- Ako lopta izađe van levog ili desnog ruba polja, poziva `ScorePoint()` za odgovarajuću stranu

---

## 6. Glavna Petlja (`main`)

1. Inicijalizacija: `srand`, kreiranje i aktivacija konzolnog bafera, sakrivanje kursora, prvi `UpdateConsoleSize()`.
2. Petlja `while (state != State::Exit)`:
   - Svaki frejm: `UpdateConsoleSize()` → `ClearBuffer()` → obrada trenutnog stanja → `Present()` → `Sleep(16)` (~60 FPS)
3. **Stanje `Menu`**: ispisuje naslov "PONG" i tri stavke ("2 PLAYER MODE", "AI MODE", "EXIT") sa `>` pokazivačem ispred selektovane. Strelice gore/dole menjaju `menuIndex`, Enter pokreće odgovarajuću akciju (`StartGame(false)`, `StartGame(true)` ili `state = Exit`).
4. **Stanje `TwoPlayers` / `AI`**: `Escape` vraća u meni; inače poziva `UpdateGame()` pa `DrawGame()`.
5. **Stanje `GameOver`**: ispisuje `winnerText` i poruku "PRESS ENTER TO RETURN TO MENU"; Enter vraća u `Menu`.

---

## 7. Ograničenja / Zapažanja Iz Koda

- Igra radi isključivo na Windows-u (koristi `<windows.h>`, `GetAsyncKeyState`, `Beep`, konzolni bafer API).
- Nema pauziranja igre (samo izlaz u meni preko `Escape`).
- Nema čuvanja najboljeg rezultata ni podešavanja (npr. `WIN_SCORE` i `PADDLE_H` su `const`, ne menjaju se iz menija).
- Kolizija reket–lopta proverava se samo na tačno definisanoj x-koordinati (`3` i `WIDTH - 4`), pa je osetljiva na brze pomene `WIDTH` ili brzinu lopte koja bi mogla "preskočiti" tu kolonu u jednom frejmu.
