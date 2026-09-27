#pragma region GLOBAL PARAMETERS
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <deque>
using namespace std;

const int FIELD_SIZE = 10;
const char WATER = '~', SHIP = '#', HIT = 'X', MISS = '.';
const vector<int> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

char playerField[FIELD_SIZE][FIELD_SIZE], aiField[FIELD_SIZE][FIELD_SIZE], aiFog[FIELD_SIZE][FIELD_SIZE];

struct Coord { int x, y; };

deque<Coord> targetQueue;
vector<Coord> recentHits;
#pragma endregion

#pragma region BOOLEANS
static bool ISInBounds(int x, int y) {
    return x >= 0 && y >= 0 && x < FIELD_SIZE && y < FIELD_SIZE;
}

static bool CanPlaceShip(char field[FIELD_SIZE][FIELD_SIZE], int x, int y, int size, bool horizontal) {
    for (int i = 0; i < size; ++i) {
        int nextX = x + (horizontal ? i : 0);
        int nextY = y + (horizontal ? 0 : i);
        if (!ISInBounds(nextX, nextY) || field[nextY][nextX] != WATER) return false;
        for (int deltaX = -1; deltaX <= 1; ++deltaX) {
            for (int deltaY = -1; deltaY <= 1; ++deltaY) {
                int checkX = nextX + deltaX;
                int checkY = nextY + deltaY;
                if (ISInBounds(checkX, checkY) && field[checkY][checkX] == SHIP) return false;
            }
        }
    }
    return true;
}

static bool MakeMove(char field[FIELD_SIZE][FIELD_SIZE], int x, int y) {
    if (!ISInBounds(x, y)) return false;
    if (field[y][x] == SHIP) {
        field[y][x] = HIT;
        return true;
    }
    else if (field[y][x] == WATER) field[y][x] = MISS;
    return false;
}

static bool AreAllShipsSunk(char field[FIELD_SIZE][FIELD_SIZE]) {
    for (int y = 0; y < FIELD_SIZE; ++y) for (int x = 0; x < FIELD_SIZE; ++x) if (field[y][x] == SHIP) return false;
    return true;
}
#pragma endregion

#pragma region VOIDS
static void PlaceShip(char field[FIELD_SIZE][FIELD_SIZE], int x, int y, int size, bool horizontal) {
    for (int i = 0; i < size; ++i) {
        int nextX = x + (horizontal ? i : 0);
        int nextY = y + (horizontal ? 0 : i);
        field[nextY][nextX] = SHIP;
    }
}

static void ClearField(char field[FIELD_SIZE][FIELD_SIZE]) {
    for (int y = 0; y < FIELD_SIZE; ++y) for (int x = 0; x < FIELD_SIZE; ++x) field[y][x] = WATER;
}

static void AutoPlaceShips(char field[FIELD_SIZE][FIELD_SIZE]) {
    srand((unsigned int)time(0));
    for (int ship : shipSizes) {
        bool placed = false;
        while (!placed) {
            int x = rand() % FIELD_SIZE;
            int y = rand() % FIELD_SIZE;
            bool horizontal = rand() % 2;
            if (CanPlaceShip(field, x, y, ship, horizontal)) {
                PlaceShip(field, x, y, ship, horizontal);
                placed = true;
            }
        }
    }
}

static void PrintField(char field[FIELD_SIZE][FIELD_SIZE], bool showShips = true) {
    cout << "  ";
    for (int x = 0; x < FIELD_SIZE; ++x) cout << x << " ";
    cout << endl;
    for (int y = 0; y < FIELD_SIZE; ++y) {
        cout << y << " ";
        for (int x = 0; x < FIELD_SIZE; ++x) {
            char c = field[y][x];
            if (!showShips && c == SHIP) c = WATER;
            cout << c << " ";
        }
        cout << endl;
    }
}

static void EnqueueDirectionalTargets() {
    if (recentHits.size() < 2) return;
    Coord first = recentHits.front();
    Coord second = recentHits.back();
    int deltaX = second.x - first.x;
    int deltaY = second.y - first.y;

    deltaX = (deltaX != 0) ? deltaX / abs(deltaX) : 0;
    deltaY = (deltaY != 0) ? deltaY / abs(deltaY) : 0;

    for (int i = 1; i <= 3; ++i) {
        int nextX = second.x + deltaX * i;
        int nextY = second.y + deltaY * i;
        if (ISInBounds(nextX, nextY) && aiFog[nextY][nextX] == WATER) targetQueue.push_front({ nextX, nextY });
        else break;
    }

    for (int i = 1; i <= 3; ++i) {
        int nextX = first.x - deltaX * i;
        int nextY = first.y - deltaY * i;
        if (ISInBounds(nextX, nextY) && aiFog[nextY][nextX] == WATER) targetQueue.push_front({ nextX, nextY });
        else break;
    }
}

static void EnqueueAdjacentTargets(int x, int y) {
    const int deltaX[] = { 1, -1, 0, 0 };
    const int deltaY[] = { 0, 0, 1, -1 };
    for (int i = 0; i < 4; ++i) {
        int nextX = x + deltaX[i];
        int nextY = y + deltaY[i];
        if (ISInBounds(nextX, nextY) && aiFog[nextY][nextX] == WATER) targetQueue.push_back({ nextX, nextY });
    }
}

static void AITurn() {
    int x, y;
    if (!targetQueue.empty()) {
        Coord target = targetQueue.front();
        targetQueue.pop_front();
        x = target.x;
        y = target.y;
    }
    else {
        do {
            x = rand() % FIELD_SIZE;
            y = rand() % FIELD_SIZE;
        } while (aiFog[y][x] == HIT || aiFog[y][x] == MISS);
        recentHits.clear();
    }

    if (MakeMove(playerField, x, y)) {
        cout << "AI Hited At (" << x << ", " << y << ")!" << endl;
        aiFog[y][x] = HIT;
        recentHits.push_back({ x, y });

        if (recentHits.size() > 1) EnqueueDirectionalTargets();
        else EnqueueAdjacentTargets(x, y);
    }
    else {
        cout << "AI Missed At (" << x << ", " << y << ")..." << endl;
        aiFog[y][x] = MISS;
        recentHits.clear();
    }
}
#pragma endregion

int main() {
    ClearField(playerField);
    ClearField(aiField);
    ClearField(aiFog);

#pragma region PLACING CYCLE
    AutoPlaceShips(aiField);
    //AutoPlaceShips(playerField); Enable For AutoPlace /ISvet/

    for (int shipSize : shipSizes) {
        bool placed = false;
        //bool placed = true; Enable For AutoPlace /ISvet/
        while (!placed) {
            cout << "Manual Placement Of Ships. Enter Coordinates And Direction(0 - Horizontal, 1 - Vertical):" << endl;
            PrintField(playerField);
            cout << "Ship Length " << shipSize << endl;
            int x, y, direction;
            cout << "X: "; cin >> x;
            cout << "Y: "; cin >> y;
            cout << "DIRECTION: "; cin >> direction;
            if (CanPlaceShip(playerField, x, y, shipSize, direction == 0)) {
                PlaceShip(playerField, x, y, shipSize, direction == 0);
                placed = true;
            }
            else {
                cout << "Can't Place Ship Here! Try Again" << endl;
                system("pause");
            }
            system("cls");
        }
    }
#pragma endregion

#pragma region GAME CYCLE
    while (true) {
        system("cls");
        cout << endl << "Player's Field:" << endl;
        PrintField(playerField);
        cout << endl << "AI's Field:" << endl;
        PrintField(aiField, false);

        cout << endl << "Your Turn. Enter Coordinates" << endl;
        int x, y;
        cout << "X: "; cin >> x;
        cout << "Y: "; cin >> y;

        if (!ISInBounds(x, y)) { cout << "Invalid Coordinates." << endl; continue; }
        if (aiField[y][x] == HIT || aiField[y][x] == MISS) { cout << "You Already Shot Here." << endl; continue; }

        if (MakeMove(aiField, x, y)) cout << "Hit!" << endl;
        else cout << "Miss..." << endl;
        system("pause");

        if (AreAllShipsSunk(aiField)) { cout << "You Won!" << endl; break; }

        AITurn();

        if (AreAllShipsSunk(playerField)) { cout << "You Lose!" << endl; break; }
    }
#pragma endregion
}
