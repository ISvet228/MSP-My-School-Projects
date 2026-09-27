# Potapanje Brodova — Konzolna Igra (C++)

Dokumentacija zasnovana isključivo na priloženom izvornom kodu.

---

## 1. Globalni Parametri

```cpp
const int FIELD_SIZE = 10;
const char WATER = '~', SHIP = '#', HIT = 'X', MISS = '.';
const vector<int> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
```

| Simbol | Vrednost | Opis |
|---|---|---|
| `FIELD_SIZE` | `10` | Dimenzija polja (10×10) |
| `WATER` | `'~'` | Prazna (negađana) ćelija |
| `SHIP` | `'#'` | Ćelija sa brodom |
| `HIT` | `'X'` | Pogođena ćelija broda |
| `MISS` | `'.'` | Gađana ćelija bez broda |
| `shipSizes` | `{4,3,3,2,2,2,1,1,1,1}` | Veličine svih 10 brodova koji se postavljaju |

### Polja i strukture

| Ime | Tip | Opis |
|---|---|---|
| `playerField` | `char[10][10]` | Stvarno polje igrača (sa brodovima) |
| `aiField` | `char[10][10]` | Stvarno polje AI (sa brodovima) |
| `aiFog` | `char[10][10]` | Ono što AI "vidi" od polja igrača — koristi se samo za AI logiku pogodaka/promašaja, ne za prikaz |
| `Coord` | `struct { int x, y; }` | Koordinata na polju |
| `targetQueue` | `deque<Coord>` | Red koordinata koje AI planira da gađa sledeće |
| `recentHits` | `vector<Coord>` | Lista uzastopnih pogodaka na trenutnom brodu koji AI gađa |

---

## 2. Logičke (bool) Funkcije

### `ISInBounds(x, y)`
Vraća `true` ako su `x` i `y` unutar opsega `0..9`.

### `CanPlaceShip(field, x, y, size, horizontal)`
Proverava da li brod dužine `size`, postavljen od `(x, y)` u datom pravcu, staje unutar polja, ne preklapa se ni sa jednom postojećom `SHIP` ćelijom **i** da nijedna od 8 susednih ćelija oko svake buduće pozicije broda nije `SHIP` (brodovi se ne smeju dodirivati, ni diagonalno).

### `MakeMove(field, x, y)`
Ako je na `(x, y)` `SHIP`, postavlja `HIT` i vraća `true`. Ako je `WATER`, postavlja `MISS` i vraća `false`. (Napomena: ako je ćelija već `HIT` ili `MISS`, funkcija ništa ne menja i vraća `false` — provera duplog gađanja radi se van ove funkcije, u glavnoj petlji za igrača.)

### `AreAllShipsSunk(field)`
Prolazi kroz celo polje; ako nijedna ćelija nije `SHIP`, vraća `true` (svi brodovi potopljeni).

---

## 3. Void Funkcije

### `PlaceShip(field, x, y, size, horizontal)`
Upisuje `SHIP` na `size` uzastopnih ćelija počev od `(x, y)`, horizontalno ili vertikalno u zavisnosti od `horizontal`. Ne proverava validnost — pretpostavlja da je `CanPlaceShip` već pozvano.

### `ClearField(field)`
Postavlja sve ćelije datog polja na `WATER`.

### `AutoPlaceShips(field)`
Za svaku veličinu iz `shipSizes`, generiše nasumičnu poziciju i orijentaciju dok `CanPlaceShip` ne vrati `true`, zatim poziva `PlaceShip`. Poziva `srand(time(0))` na početku.

### `PrintField(field, showShips = true)`
Ispisuje polje u konzolu sa koordinatnim brojevima reda i kolone. Ako je `showShips == false`, sve `SHIP` ćelije se prikazuju kao `WATER` (koristi se za prikaz AI polja igraču, da ne vidi gde su AI brodovi).

### `EnqueueAdjacentTargets(x, y)`
Nakon pogotka, dodaje u `targetQueue` (na kraj reda, `push_back`) do 4 susedne ćelije (gore, dole, levo, desno) — samo ako su unutar polja i još uvek `WATER` u `aiFog`.

### `EnqueueDirectionalTargets()`
Poziva se kada `recentHits` sadrži 2 ili više pogotka. Uzima prvi i poslednji zabeleženi pogodak, računa pravac (`deltaX`, `deltaY` normalizovani na -1/0/1) i dodaje (na početak reda, `push_front`) do 3 ćelije u tom pravcu iza poslednjeg pogotka i do 3 ćelije u suprotnom pravcu ispred prvog pogotka — zaustavlja se čim naiđe na ćeliju van polja ili već gađanu ćeliju.

### `AITurn()`
Jedan potez AI:
1. Ako `targetQueue` nije prazan, uzima sledeću metu odatle (`pop_front`).
2. Inače bira nasumičnu ćeliju koja u `aiFog` još nije `HIT` ni `MISS`, i prazni `recentHits`.
3. Poziva `MakeMove(playerField, x, y)`.
4. Ako je pogodak: ispisuje poruku, obeležava `aiFog`, dodaje koordinatu u `recentHits`, pa poziva `EnqueueDirectionalTargets()` (ako ima već 2+ pogotka) ili `EnqueueAdjacentTargets()` (ako je ovo prvi pogodak niza).
5. Ako je promašaj: ispisuje poruku, obeležava `aiFog` kao `MISS` i prazni `recentHits`.

---

## 4. Glavna Petlja (`main`)

### Priprema
- `ClearField()` se poziva za sva tri polja (`playerField`, `aiField`, `aiFog`).

### Postavljanje brodova
- `AutoPlaceShips(aiField)` — AI polje se uvek popunjava automatski.
- Igračevo polje: za svaku veličinu broda iz `shipSizes`, igrač ručno unosi `X`, `Y` i `DIRECTION` (0 = horizontalno, 1 = vertikalno) dok se ne postavi validan brod (`CanPlaceShip` provera); konzola se čisti (`system("cls")`) između pokušaja. (U kodu postoji zakomentarisana opcija da se i igračevo polje popuni automatski.)

### Ciklus igre
Petlja `while (true)`:
1. Čisti ekran, ispisuje `playerField` (sa brodovima) i `aiField` (bez brodova, preko `PrintField(aiField, false)`).
2. Igrač unosi `X`, `Y`.
3. Proverava granice (`ISInBounds`) i da li je ćelija već gađana (`HIT`/`MISS`) na `aiField` — ako jeste, traži novi unos (`continue`).
4. Poziva `MakeMove(aiField, x, y)`, ispisuje "Hit!" ili "Miss...".
5. Ako su svi AI brodovi potopljeni (`AreAllShipsSunk(aiField)`) — ispisuje "You Won!" i izlazi iz petlje.
6. Inače poziva `AITurn()`.
7. Ako su svi igračevi brodovi potopljeni (`AreAllShipsSunk(playerField)`) — ispisuje "You Lose!" i izlazi iz petlje.

---

## 5. Zapažanja Iz Koda

- Nema provere validnosti unosa `cin >> x` (npr. unos slova umesto broja nije obrađen).
- `MakeMove` ne razlikuje "već gađano" od "voda" — ta provera je urađena samo u glavnoj petlji za poteze igrača, ali ne i unutar `AITurn()` (AI bira metu isključivo iz `targetQueue` ili nasumično biranje koje eksplicitno izbegava već gađane ćelije, pa se problem praktično ne javlja).
- `srand(time(0))` se poziva unutar `AutoPlaceShips`, što znači da će se pozvati ponovo svaki put kad se ta funkcija pozove.
- Program koristi samo standardnu biblioteku (`iostream`, `cstdlib`, `vector`, `ctime`, `deque`) i `system("cls")` / `system("pause")` — zavisno od platforme (na Windows-u radi kako treba; na Linuxu `cls` ne postoji).
