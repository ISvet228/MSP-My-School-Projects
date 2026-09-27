# Fraction Calculator & Tic Tac Toe — Console Program (C++)

Documentation based strictly on the provided source code. On startup the program offers a choice between two completely separate sub-programs and three output languages.

---

## 1. Global Variables And Multilingual Support

| Variable | Type | Description |
|---|---|---|
| `GameIndex` | `int` | Selects which program runs: `<= 0` → Fraction Calculator, `>= 1` → Tic Tac Toe |
| `LanguageIndex` | `int` | Selects output language: `0` = Serbian, `1` = English, `2` = Russian |

### `Text(serbian, english, russian, isEndl)`
The central function for all multilingual output. Based on `LanguageIndex`, prints the corresponding string (`serbian`, `english`, or `russian`); if `isEndl == true`, appends a newline. Called before every prompt or message that needs translation.

**Language normalization (in `main`):** if the entered value is `< 1`, it becomes `0` (Serbian); if `> 1`, it becomes `2` (Russian); the value `1` stays English. In practice: any number below 1 (including negatives) yields Serbian, any number above 1 yields Russian, and only exactly `1` yields English.

---

## 2. `Fraction` Class

### Private fields
- `Numerator`
- `Denominator`

### Private methods
- `GCD(a, b)` — recursively computes the greatest common divisor (Euclidean algorithm).
- `Reduce()` — divides `Numerator` and `Denominator` by their GCD (simplifies the fraction).

### Constructor
`Fraction(numerator = 0, denominator = 1)` — throws `invalid_argument` if `denominator == 0`; otherwise sets the values and immediately calls `Reduce()`.

### Public methods

| Method | Description |
|---|---|
| `Input()` | Prompts the user for numerator and denominator (translated via `Text`); throws `invalid_argument` if the denominator is 0; then reduces the fraction |
| `Print()` | Prints the fraction as `numerator/denominator` |
| `PrintMixed()` | If `abs(Numerator) < Denominator`, prints the same as `Print()`; otherwise computes the whole part (`Numerator / Denominator`) and remainder (`abs(Numerator) % Denominator`) and prints a mixed number in the form `whole,remainder/denominator` (or just `whole` if the remainder is 0) |
| `operator+` | Adds two fractions using `(a*d + c*b) / (b*d)`; the result is automatically reduced by the constructor |
| `operator-` | Subtracts fractions using the same logic |
| `operator*` | Multiplies numerators and denominators |
| `operator/` | Divides fractions (multiplies by the reciprocal); throws `invalid_argument` if the divisor's numerator is 0 |

---

## 3. Tic Tac Toe

### Global variables
- `cells[3][3]` — the board, initially filled with digits `'1'`–`'9'` (used as labels for empty cells).
- `PlayerR` — move counter, starts at `1`; its parity determines whose turn it is: player 1 (`X`, odd) or player 2 (`O`, even).

### `ShowGame()`
Clears the console (`system("cls")`), prints the title (translated) and the current board state with border lines.

### `CheckWin()`
Checks all 3 rows, all 3 columns, and both diagonals — if all three cells in any line are equal (including, in theory, the case where they're still digits, though this practically never happens since the initial cells hold distinct per-cell digits), returns `true`.

### `SelectCell()`
Prints whose turn it is (based on the parity of `PlayerR`), asks for a cell number (1–9), and returns it.

### Match flow (in `main`, the `GameIndex >= 1` branch)
`while (!CheckWin())` loop:
1. `ShowGame()`.
2. Tie check: iterates over all cells; as soon as it finds a cell that is **not** `X`/`O`, it breaks the inner loop for that row (but the outer row loop continues); if it reaches the very last cell (`i==2 && b==2`) without having broken earlier, it sets `tie = true` and prints the tie message.
3. If `tie`, exits the main loop.
4. Otherwise: `SelectCell()` returns a choice, converted into `row`/`kolumn` (`(choise-1)/3`, `(choise-1)%3`).
5. If the cell is unoccupied, it's set to `X` or `O` (per the parity of `PlayerR`) and `PlayerR` is incremented; if occupied, prints a "cell already taken" message.
6. `tie` is reset to `false`, and `ShowGame()` is called again.

After the loop exits: `ShowGame()` is called once more; if `tie == false`, prints the winner based on `(PlayerR - 1) % 2` (subtracting 1 because `PlayerR` was already incremented after the last valid move); if `tie == true`, prints the tie message.

---

## 4. Main Program Flow (`main`)

1. The user enters `GameIndex` (0 = Fraction Calculator, 1 = Tic Tac Toe) and `LanguageIndex` (0/1/2), which is immediately normalized as described above.
2. **If `GameIndex <= 0` (Fraction Calculator):**
   - `while (true)` loop:
     - First pass (`FirstInputs == true`): reads two fractions and an operator, computes `Result` via the matching `operator`; if the operator is unrecognized (the switch's `default` branch), the program exits immediately (`return 0`).
     - Every subsequent pass: reads a new operator and a new fraction, applies the operation to the existing `Result`; an unrecognized operator again exits the program immediately.
     - Prints `Result` (plain and mixed form).
     - Asks the user whether to continue (`y`/anything else) — if the answer isn't exactly `'y'`, the loop breaks.
3. **If `GameIndex >= 1` (Tic Tac Toe):** runs the flow described in Section 3.

---

## 5. Notes From The Code

- `LanguageIndex` normalization happens only **after** the value is first read — the raw input (0/1/2) itself isn't validated beforehand.
- In the fraction calculator, entering an operator other than `+`, `-`, `*`, or `/` immediately ends the entire program (`return 0`), with no error message.
- `PrintMixed()` uses a comma (`,`) as the separator between the whole number and the fractional part (e.g. `2,1/3`), rather than a space or hyphen.
- In the Tic Tac Toe tie check, the inner `break` only stops the column loop for that row, while the outer row loop keeps going — the sole condition for `tie = true` is reaching the very last cell (`i==2, b==2`).
- The program uses only the standard library (`iostream`) plus `system("cls")`, making it platform-dependent (works on Windows; the `cls` command does not exist on Linux).
