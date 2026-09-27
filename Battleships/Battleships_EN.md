# Battleship — Console Game (C++)

Documentation based strictly on the provided source code.

---

## 1. Global Parameters

```cpp
const int FIELD_SIZE = 10;
const char WATER = '~', SHIP = '#', HIT = 'X', MISS = '.';
const vector<int> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
```

| Symbol | Value | Description |
|---|---|---|
| `FIELD_SIZE` | `10` | Board dimension (10×10) |
| `WATER` | `'~'` | Empty (unfired-at) cell |
| `SHIP` | `'#'` | Cell occupied by a ship |
| `HIT` | `'X'` | Hit cell (ship was there) |
| `MISS` | `'.'` | Fired-at cell with no ship |
| `shipSizes` | `{4,3,3,2,2,2,1,1,1,1}` | Sizes of all 10 ships to be placed |

### Boards and structures

| Name | Type | Description |
|---|---|---|
| `playerField` | `char[10][10]` | Player's actual board (with ships) |
| `aiField` | `char[10][10]` | AI's actual board (with ships) |
| `aiFog` | `char[10][10]` | What the AI "sees" of the player's board — used only for AI hit/miss tracking, not for display |
| `Coord` | `struct { int x, y; }` | A board coordinate |
| `targetQueue` | `deque<Coord>` | Queue of coordinates the AI plans to fire at next |
| `recentHits` | `vector<Coord>` | List of consecutive hits on the ship the AI is currently targeting |

---

## 2. Boolean Functions

### `ISInBounds(x, y)`
Returns `true` if `x` and `y` are within the `0..9` range.

### `CanPlaceShip(field, x, y, size, horizontal)`
Checks whether a ship of length `size`, placed from `(x, y)` in the given direction, fits inside the board, does not overlap any existing `SHIP` cell, **and** that none of the 8 neighboring cells around every future ship cell is `SHIP` (ships may not touch, not even diagonally).

### `MakeMove(field, x, y)`
If `(x, y)` is `SHIP`, sets it to `HIT` and returns `true`. If it's `WATER`, sets it to `MISS` and returns `false`. (Note: if the cell is already `HIT` or `MISS`, the function leaves it unchanged and returns `false` — the "already shot here" check is done outside this function, in the main loop for the player's turn.)

### `AreAllShipsSunk(field)`
Scans the entire board; if no cell is `SHIP`, returns `true` (all ships sunk).

---

## 3. Void Functions

### `PlaceShip(field, x, y, size, horizontal)`
Writes `SHIP` into `size` consecutive cells starting at `(x, y)`, horizontally or vertically depending on `horizontal`. Does not validate — assumes `CanPlaceShip` was already called.

### `ClearField(field)`
Sets every cell of the given board to `WATER`.

### `AutoPlaceShips(field)`
For each size in `shipSizes`, generates a random position and orientation until `CanPlaceShip` returns `true`, then calls `PlaceShip`. Calls `srand(time(0))` at the start.

### `PrintField(field, showShips = true)`
Prints the board to the console with row/column coordinate numbers. If `showShips == false`, every `SHIP` cell is displayed as `WATER` (used to show the AI's board to the player without revealing AI ship positions).

### `EnqueueAdjacentTargets(x, y)`
After a hit, appends (`push_back`) up to 4 adjacent cells (up, down, left, right) to `targetQueue` — only if they're within bounds and still `WATER` in `aiFog`.

### `EnqueueDirectionalTargets()`
Called when `recentHits` holds 2 or more hits. Takes the first and last recorded hit, computes the direction (`deltaX`, `deltaY` normalized to -1/0/1), and pushes (`push_front`) up to 3 cells in that direction beyond the last hit, and up to 3 cells in the opposite direction before the first hit — stopping as soon as it reaches an out-of-bounds or already-fired-at cell.

### `AITurn()`
One AI turn:
1. If `targetQueue` is not empty, pops the next target from it (`pop_front`).
2. Otherwise picks a random cell that is not yet `HIT` or `MISS` in `aiFog`, and clears `recentHits`.
3. Calls `MakeMove(playerField, x, y)`.
4. On hit: prints a message, marks `aiFog`, appends the coordinate to `recentHits`, then calls `EnqueueDirectionalTargets()` (if 2+ hits so far) or `EnqueueAdjacentTargets()` (if this is the first hit of the streak).
5. On miss: prints a message, marks `aiFog` as `MISS`, and clears `recentHits`.

---

## 4. Main Loop (`main`)

### Setup
- `ClearField()` is called for all three boards (`playerField`, `aiField`, `aiFog`).

### Ship placement
- `AutoPlaceShips(aiField)` — the AI's board is always filled automatically.
- Player's board: for each ship size in `shipSizes`, the player manually enters `X`, `Y`, and `DIRECTION` (0 = horizontal, 1 = vertical) until a valid ship is placed (checked via `CanPlaceShip`); the console is cleared (`system("cls")`) between attempts. (The code contains a commented-out option to auto-place the player's board as well.)

### Game cycle
`while (true)` loop:
1. Clears the screen, prints `playerField` (with ships) and `aiField` (without ships, via `PrintField(aiField, false)`).
2. Player enters `X`, `Y`.
3. Checks bounds (`ISInBounds`) and whether the cell on `aiField` was already fired at (`HIT`/`MISS`) — if so, prompts again (`continue`).
4. Calls `MakeMove(aiField, x, y)`, prints "Hit!" or "Miss...".
5. If all AI ships are sunk (`AreAllShipsSunk(aiField)`) — prints "You Won!" and exits the loop.
6. Otherwise calls `AITurn()`.
7. If all of the player's ships are sunk (`AreAllShipsSunk(playerField)`) — prints "You Lose!" and exits the loop.

---

## 5. Notes From The Code

- No input validation on `cin >> x` (e.g., entering a letter instead of a number is not handled).
- `MakeMove` does not distinguish "already fired at" from "water" — that check is done only in the main loop for the player's moves, not inside `AITurn()` (the AI only picks targets from `targetQueue`, or via a random pick that explicitly avoids already-fired-at cells, so the issue doesn't practically occur there).
- `srand(time(0))` is called inside `AutoPlaceShips`, meaning it re-seeds every time that function is called.
- The program uses only the standard library (`iostream`, `cstdlib`, `vector`, `ctime`, `deque`) plus `system("cls")` / `system("pause")` — platform-dependent (works as intended on Windows; `cls` does not exist on Linux).
