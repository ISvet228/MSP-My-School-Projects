#include <iostream>
using namespace std;

int GameIndex;
int LanguageIndex;
void Text(string serbian, string english, string russian, bool isEndl) {
    switch (LanguageIndex)
    {
        case 0: cout << serbian; break; case 1: cout << english; break; case 2: cout << russian; break;
    }
    if (isEndl)cout << endl;
}

#pragma region Fraction Calculator V2.206L
class Fraction {
private:
    int Numerator;
    int Denominator;

    int GCD(int a, int b) const { return b == 0 ? a : GCD(b, a % b); }

    void Reduce() {
        int g = GCD(Numerator, Denominator);
        Numerator /= g;
        Denominator /= g;
    }
public:
    Fraction(int numerator = 0, int denominator = 1) {
        if (denominator == 0) throw invalid_argument("Denominator cannot be zero");
        Numerator = numerator;
        Denominator = denominator;
        Reduce();
    }
    void Input() {
        Text("Unesite brojilac: ", "Enter numerator: ", "Введите числитель: ", false); cin >> Numerator;
        Text("Unesite imenilac: ", "Enter denominator: ", "Введите знаменатель: ", false); cin >> Denominator;
        if (Denominator == 0) throw invalid_argument("Denominator cannot be zero");
        Reduce();
    }
    void Print() const { cout << Numerator << "/" << Denominator; }
    void PrintMixed() const {
        if (abs(Numerator) < Denominator) Print();
        else {
            int whole = Numerator / Denominator;
            int remainder = abs(Numerator) % Denominator;
            if (remainder == 0) cout << whole;
            else cout << whole << "," << remainder << "/" << Denominator;
        }
    }
    Fraction operator+(const Fraction& fraction) const {
        int numerator = Numerator * fraction.Denominator + fraction.Numerator * Denominator;
        int denominator = Denominator * fraction.Denominator;
        return Fraction(numerator, denominator);
    }
    Fraction operator-(const Fraction& fraction) const {
        int numerator = Numerator * fraction.Denominator - fraction.Numerator * Denominator;
        int denominator = Denominator * fraction.Denominator;
        return Fraction(numerator, denominator);
    }
    Fraction operator*(const Fraction& fraction) const {
        int numerator = Numerator * fraction.Numerator;
        int denominator = Denominator * fraction.Denominator;
        return Fraction(numerator, denominator);
    }
    Fraction operator/(const Fraction& fraction) const {
        if (fraction.Numerator == 0) throw invalid_argument("Division by zero");
        int numerator = Numerator * fraction.Denominator;
        int denominator = Denominator * fraction.Numerator;
        return Fraction(numerator, denominator);
    }
};
#pragma endregion

#pragma region Tic Tac Toe
char cells[3][3] = { {'1', '2', '3'}, {'4', '5', '6'},{ '7', '8', '9'} };
int PlayerR = 1;

void ShowGame() {
    system("cls");
    Text("   Iks Oks", "   Tic Tac Toe", "   Крестики-нолики", true);
    cout << "-------------" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "| ";
        for (int j = 0; j < 3; j++)cout << cells[i][j] << " | ";
        cout << endl;
        cout << "-------------" << endl;
    }
}
bool CheckWin() {
    for (int i = 0; i < 3; i++) { if (cells[i][0] == cells[i][1] && cells[i][1] == cells[i][2])return true; }
    for (int i = 0; i < 3; i++) { if (cells[0][i] == cells[1][i] && cells[1][i] == cells[2][i])return true; }
    if (cells[0][0] == cells[1][1] && cells[1][1] == cells[2][2])return true;
    if (cells[0][2] == cells[1][1] && cells[1][1] == cells[2][0])return true;
    return false;
}
int SelectCell() {
    int choise;
    Text("Igrac ", "Player ", "Игрок ", false);
    cout << (PlayerR % 2 == 1 ? "1 (X)" : "2 (O)");
    Text(", unesite broj polja: ", ", enter the cell number: ", ", введите номер ячейки: ", false);
    cin >> choise;
    return choise;
}
#pragma endregion

int main() {
    cout << "Chose the program(0 - Fraction Calculator, 1 - Tic Tac Toe): "; cin >> GameIndex;

    cout << "Select Language(0 serbian, 1 english, 2 russian): "; cin >> LanguageIndex; cout << endl;

    if (LanguageIndex < 1)LanguageIndex = 0;
    else if (LanguageIndex > 1)LanguageIndex = 2;

    if (GameIndex <= 0) {
        Fraction Result;
        char Operation;
        bool FirstInputs = true;

        while (true) {
            if (FirstInputs) {
                Fraction FirstFraction, SecondFraction;

                Text("Unesite prvi razlomak", "Enter first fraction", "Введите первую дробь", true); FirstFraction.Input(); cout << endl;
                Text("Unesite operaciju (+, -, *, /): ", "Enter operator (+, -, *, /): ", "Выберите оператор (+, -, *, /): ", false); /*ISvet*/ cin >> Operation; cout << endl;
                Text("Unesite drugi razlomak", "Enter second fraction", "Введите вторую дробь", true); SecondFraction.Input(); cout << endl;

                switch (Operation) {
                    case '+': Result = FirstFraction + SecondFraction; break;
                    case '-': Result = FirstFraction - SecondFraction; break;
                    case '*': Result = FirstFraction * SecondFraction; break;
                    case '/': Result = FirstFraction / SecondFraction; break;
                    default: return 0;
                }
                FirstInputs = false;

            }
            else {
                Fraction NextFraction;

                Text("Unesite operaciju (+, -, *, /): ", "Enter operator (+, -, *, /): ", "Выберите оператор (+, -, *, /): ", false); cin >> Operation; cout << endl;
                Text("Unesite sledeci razlomak", "Enter next fraction", "Введи новую дробь", true); NextFraction.Input(); cout << endl;

                switch (Operation) {
                    case '+': Result = Result + NextFraction; break;
                    case '-': Result = Result - NextFraction; break;
                    case '*': Result = Result * NextFraction; break;
                    case '/': Result = Result / NextFraction; break;
                    default:return 0;
                }
            }

            Text("Rezultat: ", "Result: ", "Результат: ", false); Result.Print();

            Text(" (Mesoviti: ", " (Mixed: ", " (Смешаный: ", false); Result.PrintMixed(); cout << ")" << endl;

            char Choice;

            Text("Da li zelite da nastavite sa jos operacija? (y/n): ", "Do you want to continue with more operations? (y/n): ", "Хотите ли вы продолжить с расчётными операциями? (y/n): ", false);

            cin >> Choice;
            if (Choice != 'y') break;
        }
    }

    else if (GameIndex >= 1) {
        int row, kolumn, choise;
        bool tie = false;
        while (!CheckWin()) {
            ShowGame();
            for (int i = 0; i < 3; i++) {
                for (int b = 0; b < 3; b++) {
                    if ((cells[i][b] == 'X' || cells[i][b] == 'O') == false) break;
                    if (i == 2 && b == 2) {
                        tie = true;
                        Text("Nereseno", "Tie", "Ничья", true);
                    }
                }
                if (tie)break;
            }
            if (tie) break;
            choise = SelectCell();
            row = (choise - 1) / 3;
            kolumn = (choise - 1) % 3;

            if (cells[row][kolumn] != 'X' && cells[row][kolumn] != 'O') { cells[row][kolumn] = (PlayerR % 2 == 1) ? 'X' : 'O'; PlayerR++; }
            else Text("Polje je vec popunjeno! Pokusajte ponovo.", "Cell is already taken! Try again.", "Ячейка уже занята! Попробуйте снова.", true);
            tie = false;
            ShowGame();
        }
        ShowGame();
        if (!tie) {
            Text("Pobednik je igrac ", "The winner is player ", "Победитель игрок ", false);
            cout << ((PlayerR - 1) % 2 == 1 ? "1 (X)" : "2 (O)") << "! ";
            Text("Cestitamo!", "Congratulations!", "Поздравляем!", true);
        }
        else Text("Nereseno", "Tie", "Ничья", true);
    }
}
