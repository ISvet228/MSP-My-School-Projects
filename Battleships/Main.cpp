#include <windows.h>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
using namespace std;

int WIDTH = 80;
int HEIGHT = 25;
const int WIN_SCORE = 10;
const int PADDLE_H = 4;

enum class State { Menu, TwoPlayers, AI, GameOver, Exit };
struct Paddle { float y; };
struct Ball { float x, y, vx, vy; };
HANDLE hConsole;
vector<char> screen;

static void UpdateConsoleSize();
static void ResetBall();

State state = State::Menu;
int menuIndex = 0;

Paddle leftPad, rightPad;
Ball ball;

int leftScore = 0;
int rightScore = 0;
bool aiMode = false;
string winnerText;

float aiSpeed = 0.15f;

static void ClearBuffer() { if (!screen.empty()) fill(screen.begin(), screen.end(), ' '); }
static void Put(int x, int y, char c) { if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && !screen.empty()) screen[y * WIDTH + x] = c; }
static void DrawText(int x, int y, const string& s) { for (size_t i = 0; i < s.size(); i++) Put(x + (int)i, y, s[i]); }
static void Present() {
    if (screen.empty()) return;
    DWORD written;
    COORD pos = { 0,0 };
    WriteConsoleOutputCharacterA(hConsole, screen.data(), (DWORD)(WIDTH * HEIGHT), pos, &written);
}
static void UpdateConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (hConsole && GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        int newWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int newHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        if (newWidth < 20) newWidth = 20;
        if (newHeight < 5) newHeight = 5;
        if (newWidth != WIDTH || newHeight != HEIGHT) {
            WIDTH = newWidth;
            HEIGHT = newHeight;
            screen.assign(static_cast<vector<char, allocator<char>>::size_type>(WIDTH) * HEIGHT, ' ');
            if (leftPad.y < 1) leftPad.y = 1;
            if (rightPad.y < 1) rightPad.y = 1;
            if (leftPad.y > HEIGHT - PADDLE_H - 1) leftPad.y = HEIGHT - PADDLE_H - 1;
            if (rightPad.y > HEIGHT - PADDLE_H - 1) rightPad.y = HEIGHT - PADDLE_H - 1;
            if (ball.x < 0 || ball.x >= WIDTH || ball.y < 0 || ball.y >= HEIGHT) ResetBall();
        }
    }
}
static void ResetBall() {
    ball.x = WIDTH / 2.0f;
    ball.y = HEIGHT / 2.0f;
    ball.vx = (rand() % 2 ? 0.45f : -0.45f);
    ball.vy = ((rand() % 200) - 100) / 300.0f;
}
static void StartGame(bool vsAI) {
    aiMode = vsAI;
    leftScore = rightScore = 0;
    leftPad.y = rightPad.y = HEIGHT / 2 - PADDLE_H / 2;
    aiSpeed = 0.15f;
    ResetBall();
    state = vsAI ? State::AI : State::TwoPlayers;
}
static bool KeyPressed(int vk) {
    static SHORT prev[256] = {};
    SHORT cur = GetAsyncKeyState(vk);
    bool pressed = (cur & 0x8000) && !(prev[vk] & 0x8000);
    prev[vk] = cur;
    return pressed;
}
static void DrawBorders() {
    for (int x = 0; x < WIDTH; x++) {
        Put(x, 0, '#');
        Put(x, HEIGHT - 1, '#');
    }
    for (int y = 1; y < HEIGHT - 1; y++) Put(WIDTH / 2, y, '|');
}
static void DrawGame() {
    DrawBorders();
    for (int i = 0; i < PADDLE_H; i++) {
        Put(2, (int)leftPad.y + i, '|');
        Put(WIDTH - 3, (int)rightPad.y + i, '|');
    }
    Put((int)ball.x, (int)ball.y, 'O');
    DrawText(2, 1, to_string(leftScore));
    DrawText(WIDTH - 5, 1, to_string(rightScore));
}
static void ScorePoint(bool leftPlayer) {
    if (leftPlayer) leftScore++;
    else rightScore++;
    if (aiMode) aiSpeed += 0.02f;
    if (leftScore >= WIN_SCORE || rightScore >= WIN_SCORE) {
        if (aiMode) winnerText = leftScore > rightScore ? "PLAYER WINS!" : "AI WINS!";
        else winnerText = leftScore > rightScore ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!";
        state = State::GameOver;
    }
    ResetBall();
}
static void UpdateGame() {
    if (GetAsyncKeyState('W') & 0x8000) leftPad.y -= 0.35f;
    if (GetAsyncKeyState('S') & 0x8000) leftPad.y += 0.35f;
    if (!aiMode) {
        if (GetAsyncKeyState(VK_UP) & 0x8000) rightPad.y -= 0.35f;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) rightPad.y += 0.35f;
    }
    else {
        float center = rightPad.y + PADDLE_H / 2.0f;
        if (ball.y > center + 1) rightPad.y += aiSpeed;
        if (ball.y < center - 1) rightPad.y -= aiSpeed;
    }
    if (leftPad.y < 1) leftPad.y = 1;
    if (rightPad.y < 1) rightPad.y = 1;
    if (leftPad.y > HEIGHT - PADDLE_H - 1) leftPad.y = HEIGHT - PADDLE_H - 1;
    if (rightPad.y > HEIGHT - PADDLE_H - 1) rightPad.y = HEIGHT - PADDLE_H - 1;
    ball.x += ball.vx;
    ball.y += ball.vy;
    if (ball.y <= 1 || ball.y >= HEIGHT - 2) ball.vy = -ball.vy;
    if ((int)ball.x == 3) {
        if ((int)ball.y >= (int)leftPad.y && (int)ball.y < (int)leftPad.y + PADDLE_H) {
            ball.vx = -ball.vx * 1.05f;
            Beep(900, 10);
        }
    }
    if ((int)ball.x == WIDTH - 4) {
        if ((int)ball.y >= (int)rightPad.y && (int)ball.y < (int)rightPad.y + PADDLE_H) {
            ball.vx = -ball.vx * 1.05f;
            Beep(900, 10);
        }
    }
    if (ball.x < 0) ScorePoint(false);
    if (ball.x > WIDTH - 1) ScorePoint(true);
}
int main() {
    srand(GetTickCount64());
    hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize = 100;
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &ci);
    UpdateConsoleSize();
    while (state != State::Exit) {
        UpdateConsoleSize();
        ClearBuffer();
        if (state == State::Menu) {
            int cx = WIDTH / 2;
            int cy = HEIGHT / 2;
            string title = "PONG";
            DrawText(cx - (int)title.size() / 2, cy - 5, title);
            string a = menuIndex == 0 ? "> 2 PLAYER MODE" : "  2 PLAYER MODE";
            string b = menuIndex == 1 ? "> AI MODE" : "  AI MODE";
            string c = menuIndex == 2 ? "> EXIT" : "  EXIT";
            DrawText(cx - (int)a.size() / 2, cy - 1, a);
            DrawText(cx - (int)b.size() / 2, cy + 1, b);
            DrawText(cx - (int)c.size() / 2, cy + 3, c);

            if (KeyPressed(VK_UP)) menuIndex = (menuIndex + 2) % 3;
            if (KeyPressed(VK_DOWN)) menuIndex = (menuIndex + 1) % 3;
            if (KeyPressed(VK_RETURN)) {
                if (menuIndex == 0) StartGame(false);
                if (menuIndex == 1) StartGame(true);
                if (menuIndex == 2) state = State::Exit;
            }
        }
        else if (state == State::TwoPlayers || state == State::AI) {
            if (KeyPressed(VK_ESCAPE)) state = State::Menu;
            UpdateGame();
            DrawGame();
        }
        else if (state == State::GameOver) {
            int cx = WIDTH / 2;
            int cy = HEIGHT / 2;
            DrawText(cx - (int)winnerText.size() / 2, cy - 1, winnerText);
            string prompt = "PRESS ENTER TO RETURN TO MENU";
            DrawText(cx - (int)prompt.size() / 2, cy + 1, prompt);
            if (KeyPressed(VK_RETURN)) state = State::Menu;
        }
        Present();
        Sleep(16);
    }
}
