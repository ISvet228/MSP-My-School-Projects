# Kalkulator Razlomaka i Iks-Oks — Konzolni Program (C++)

Dokumentacija zasnovana isključivo na priloženom izvornom kodu. Program pri pokretanju nudi izbor između dva potpuno odvojena programa i tri jezika ispisa.

---

## 1. Globalne Promenljive i Višejezična Podrška

| Promenljiva | Tip | Opis |
|---|---|---|
| `GameIndex` | `int` | Bira koji se program pokreće: `<= 0` → Kalkulator razlomaka, `>= 1` → Iks-Oks |
| `LanguageIndex` | `int` | Bira jezik ispisa: `0` = srpski, `1` = engleski, `2` = ruski |

### `Text(serbian, english, russian, isEndl)`
Centralna funkcija za sav višejezični ispis. Na osnovu vrednosti `LanguageIndex` ispisuje odgovarajući string (`serbian`, `english` ili `russian`); ako je `isEndl == true`, dodaje novi red na kraju. Poziva se pre svakog ulaza ili poruke koja treba da bude prevedena.

**Normalizacija jezika (u `main`):** ako je uneta vrednost `< 1`, postaje `0` (srpski); ako je `> 1`, postaje `2` (ruski); vrednost `1` ostaje engleski. Praktično: bilo koji broj manji od 1 (uključujući negativne) daje srpski, bilo koji veći od 1 daje ruski, samo tačno `1` daje engleski.

---

## 2. Klasa `Fraction` (Razlomak)

### Privatna polja
- `Numerator` — brojilac
- `Denominator` — imenilac

### Privatne metode
- `GCD(a, b)` — rekurzivno računa najveći zajednički delilac (Euklidov algoritam).
- `Reduce()` — deli `Numerator` i `Denominator` sa njihovim NZD-om (skraćuje razlomak).

### Konstruktor
`Fraction(numerator = 0, denominator = 1)` — baca `invalid_argument` ako je `denominator == 0`; u suprotnom postavlja vrednosti i odmah poziva `Reduce()`.

### Javne metode

| Metoda | Opis |
|---|---|
| `Input()` | Traži od korisnika brojilac i imenilac (prevedeno preko `Text`); baca `invalid_argument` ako je imenilac 0; zatim skraćuje razlomak |
| `Print()` | Ispisuje razlomak u obliku `brojilac/imenilac` |
| `PrintMixed()` | Ako je `abs(Numerator) < Denominator`, ispisuje isto kao `Print()`; inače računa ceo broj (`Numerator / Denominator`) i ostatak (`abs(Numerator) % Denominator`) i ispisuje kao mešoviti broj u obliku `celo,ostatak/imenilac` (ili samo `celo` ako je ostatak 0) |
| `operator+` | Sabira dva razlomka po formuli `(a*d + c*b) / (b*d)`, rezultat se automatski skraćuje kroz konstruktor |
| `operator-` | Oduzima razlomke po istoj logici |
| `operator*` | Množi brojioce i imenioce |
| `operator/` | Deli razlomke (množenje sa recipročnom vrednošću); baca `invalid_argument` ako je brojilac delioca 0 |

---

## 3. Iks-Oks (Tic Tac Toe)

### Globalne promenljive
- `cells[3][3]` — tabla, inicijalno popunjena brojevima `'1'`–`'9'` (koriste se kao oznake praznih polja).
- `PlayerR` — brojač poteza, počinje na `1`; parnost određuje na potezu je igrač 1 (`X`, neparno) ili igrač 2 (`O`, parno).

### `ShowGame()`
Čisti konzolu (`system("cls")`), ispisuje naslov (prevedeno) i trenutno stanje table sa graničnim linijama.

### `CheckWin()`
Proverava sve 3 vrste, sve 3 kolone i obe dijagonale — ako su sve tri ćelije u nekoj liniji jednake (uključujući i slučaj kad su to i dalje brojevi, mada se to praktično ne dešava jer su početne vrednosti različite cifre po ćeliji), vraća `true`.

### `SelectCell()`
Ispisuje kome je red (na osnovu parnosti `PlayerR`), traži broj polja (1–9) i vraća ga.

### Tok partije (u `main`, deo `GameIndex >= 1`)
Petlja `while (!CheckWin())`:
1. `ShowGame()`.
2. Provera nerešenog rezultata: prolazi kroz sve ćelije; čim naiđe na ćeliju koja **nije** `X`/`O`, prekida unutrašnju petlju za taj red (ali spoljašnja petlja po redovima se nastavlja); ako se dođe do poslednje ćelije (`i==2 && b==2`) a da ranije nije bilo prekida, postavlja `tie = true` i ispisuje poruku o nerešenom rezultatu.
3. Ako je `tie`, izlazi iz glavne petlje.
4. Inače: `SelectCell()` vraća izbor, koji se pretvara u `row`/`kolumn` (`(choise-1)/3`, `(choise-1)%3`).
5. Ako ćelija nije zauzeta, upisuje se `X` ili `O` (prema parnosti `PlayerR`) i `PlayerR` se uvećava; ako je zauzeta, ispisuje poruku o zauzetom polju.
6. `tie` se resetuje na `false`, poziva se `ShowGame()` ponovo.

Po izlasku iz petlje: poziva se `ShowGame()` još jednom; ako `tie == false`, ispisuje se pobednik na osnovu `(PlayerR - 1) % 2` (umanjeno za 1 jer je `PlayerR` već uvećan posle poslednjeg validnog poteza); ako je `tie == true`, ispisuje se poruka o nerešenom rezultatu.

---

## 4. Glavni Tok Programa (`main`)

1. Korisnik unosi `GameIndex` (0 = Kalkulator razlomaka, 1 = Iks-Oks) i `LanguageIndex` (0/1/2), koji se odmah normalizuje na opisan način.
2. **Ako je `GameIndex <= 0` (Kalkulator razlomaka):**
   - Petlja `while (true)`:
     - Prvi prolaz (`FirstInputs == true`): učitava dva razlomka i operator, računa `Result` preko odgovarajućeg `operator`; ako je operator nepoznat (default grana switch-a), program se odmah završava (`return 0`).
     - Svaki naredni prolaz: učitava novi operator i novi razlomak, primenjuje operaciju na postojeći `Result`; nepoznat operator opet odmah završava program.
     - Ispisuje `Result` (obični i mešoviti oblik).
     - Pita korisnika da li želi da nastavi (`y`/bilo šta drugo) — ako odgovor nije tačno `'y'`, petlja se prekida.
3. **Ako je `GameIndex >= 1` (Iks-Oks):** izvršava se tok opisan u sekciji 3.

---

## 5. Zapažanja Iz Koda

- `LanguageIndex` normalizacija je urađena tek **posle** prvog čitanja vrednosti — sam unos (0/1/2) se ne validira pre toga.
- U kalkulatoru razlomaka, unos operatora koji nije `+`, `-`, `*` ili `/` odmah završava ceo program (`return 0`), bez ikakve poruke o grešci.
- `PrintMixed()` koristi zarez (`,`) kao razdvajač između celog broja i razlomačkog dela (npr. `2,1/3`), a ne razmak ili crticu.
- U proveri nerešenog rezultata u Iks-Oks petlji, unutrašnji `break` prekida samo petlju po koloni za taj red, dok se spoljašnja petlja po redovima nastavlja — provera da li je baš poslednja ćelija (`i==2, b==2`) dostignuta je jedini uslov za `tie = true`.
- Program koristi samo standardnu biblioteku (`iostream`) i `system("cls")`, što ga čini zavisnim od platforme (radi na Windows-u; na Linuxu `cls` komanda ne postoji).
