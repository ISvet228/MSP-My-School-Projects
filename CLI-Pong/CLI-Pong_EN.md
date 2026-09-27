# Pong — Console Game (C++ / Windows Console)

Documentation based strictly on the provided source code (`.cpp`, Windows Console API).

---

## 1. Headers And Configuration

```cpp
#include <windows.h>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
```

| Symbol | Value / Type | Description |
|---|---|---|
| `WIDTH` | `int`, starts at `80` | Console field width (changes dynamically with window size) |
| `HEIGHT` | `int`, starts at `25` | Console field height (changes dynamically with window size) |
| `WIN_SCORE` | `const int = 10` | Points needed to win |
| `PADDLE_H` | `const int = 4` | Paddle height, in characters |

---

## 2. Enum And Structures

### `enum class State`
```cpp
Menu, TwoPlayers, AI, GameOver, Exit
```
Represents the current game state (state machine).

### `struct Paddle`
```cpp
float y;
```
Vertical position of a paddle.

### `struct Ball`
```cpp
float x, y, vx, vy;
```
Ball position (`x`, `y`) and velocity on each axis (`vx`, `vy`).

---

## 3. Global Variables

| Variable | Type | Description |
|---|---|---|
| `hConsole` | `HANDLE` | Handle to the console screen buffer used for drawing |
| `screen` | `vector<char>` | Character buffer printed to the console (size `WIDTH * HEIGHT`) |
| `state` | `State` | Current game state, starts at `Menu` |
| `menuIndex` | `int` | Index of the currently selected menu item (0–2) |
| `leftPad`, `rightPad` | `Paddle` | Left and right paddles |
| `ball` | `Ball` | The ball |
| `leftScore`, `rightScore` | `int` | Current score for each side |
| `aiMode` | `bool` | Whether playing against AI (`true`) or another player (`false`) |
| `winnerText` | `string` | Text shown on the game-over screen |
| `aiSpeed` | `float`, starts at `0.15f` | AI paddle reaction speed; increases as the match goes on |

---

## 4. Drawing Functions

| Function | What it does |
|---|---|
| `ClearBuffer()` | Fills the entire `screen` buffer with spaces (`' '`) |
| `Put(x, y, c)` | Writes a single character `c` at `(x, y)` in the buffer, with bounds checking |
| `DrawText(x, y, s)` | Prints the string `s` character by character starting at `(x, y)`, calling `Put` |
| `Present()` | Sends the `screen` buffer contents to the console via `WriteConsoleOutputCharacterA` |
| `DrawBorders()` | Draws the top and bottom edges (`#`) and the middle line (`|`) of the field |
| `DrawGame()` | Calls `DrawBorders`, draws both paddles, the ball (`O`), and prints both players' scores |

---

## 5. Game Logic Functions

### `UpdateConsoleSize()`
Reads the actual console window size (`GetConsoleScreenBufferInfo`). If it changed, updates `WIDTH`/`HEIGHT`, resizes `screen`, clamps paddle positions within the new bounds, and calls `ResetBall()` if the ball ended up outside the field.

### `ResetBall()`
Places the ball at the center of the field and gives it a random direction:
- `vx` = `+0.45f` or `-0.45f` (random)
- `vy` = a random value roughly in the range `[-0.33f, 0.33f]`

### `StartGame(vsAI)`
Resets both scores to 0, centers both paddles, resets `aiSpeed` to `0.15f`, calls `ResetBall()`, and sets `state` to `AI` or `TwoPlayers` depending on the `vsAI` parameter.

### `KeyPressed(vk)`
Detects a **single key press** (not a held key) using `GetAsyncKeyState` and a static array of previous states per virtual key code (`prev[256]`).

### `ScorePoint(leftPlayer)`
Increments the corresponding side's score. If `aiMode` is active, increases `aiSpeed` by `0.02f` (the game gets harder). If either side reaches `WIN_SCORE`, sets `winnerText` (the text depends on whether AI mode is on) and switches `state` to `GameOver`. Always calls `ResetBall()` at the end.

### `UpdateGame()`
Main per-frame physics and logic:
- Left paddle: `W`/`S` (held keys, not "single press")
- Right paddle: up/down arrows — **only if `aiMode == false`**
- If `aiMode == true`, the AI moves `rightPad` toward the ball's vertical center at `aiSpeed` (simply tracks the ball's vertical position)
- Clamps both paddles within the field bounds
- Moves the ball (`ball.x += vx`, `ball.y += vy`)
- Bounces the ball off the top/bottom edges (`vy = -vy`)
- Detects paddle collision (at fixed x-positions `3` and `WIDTH - 4`): if the ball hits a paddle, `vx` flips sign and **speeds up by 5%** (`* 1.05f`), with a `Beep(900, 10)` sound
- If the ball leaves the field on the left or right, calls `ScorePoint()` for the corresponding side

---

## 6. Main Loop (`main`)

1. Initialization: `srand`, create and activate the console screen buffer, hide the cursor, first call to `UpdateConsoleSize()`.
2. Loop `while (state != State::Exit)`:
   - Every frame: `UpdateConsoleSize()` → `ClearBuffer()` → handle the current state → `Present()` → `Sleep(16)` (~60 FPS)
3. **`Menu` state**: prints the title "PONG" and three items ("2 PLAYER MODE", "AI MODE", "EXIT") with a `>` marker in front of the selected one. Up/down arrows change `menuIndex`; Enter triggers the corresponding action (`StartGame(false)`, `StartGame(true)`, or `state = Exit`).
4. **`TwoPlayers` / `AI` state**: `Escape` returns to the menu; otherwise calls `UpdateGame()` then `DrawGame()`.
5. **`GameOver` state**: prints `winnerText` and the message "PRESS ENTER TO RETURN TO MENU"; Enter returns to `Menu`.

---

## 7. Notes From The Code

- The game is Windows-only (uses `<windows.h>`, `GetAsyncKeyState`, `Beep`, the console buffer API).
- No pause functionality (only exiting to the menu via `Escape`).
- No high-score saving or in-menu settings (`WIN_SCORE` and `PADDLE_H` are `const`, not adjustable from the menu).
- Paddle-ball collision is checked only at an exact x-coordinate (`3` and `WIDTH - 4`), so it's sensitive to sudden changes in `WIDTH` or ball speed that could make it "skip" that column in a single frame.
