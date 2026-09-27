#include <iostream>
#include <cstdlib>
using namespace std;

int LanguageIndex;

class Fraction {
private:
    int Numerator;
    int Denominator;

    int GCD(int a, int b) const { return b == 0 ? a : GCD(b, a % b);}

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
        switch (LanguageIndex) {
            case 0: cout << "Unesite brojilac: "; break;
            case 1: cout << "Enter numerator: "; break;
            case 2: cout << "Введите числитель: "; break;
        }
        cin >> Numerator;
        switch (LanguageIndex) {
            case 0: cout << "Unesite imenilac: "; break;
            case 1: cout << "Enter Denominator: "; break;
            case 2: cout << "Введите знаменатель: "; break;
        }
        cin >> Denominator;
        if (Denominator == 0) throw invalid_argument("Denominator cannot be zero");
        Reduce();
    }
    void Print() const {
        cout << Numerator << "/" << Denominator;
    }
    void PrintMixed() const {
        if (abs(Numerator) < Denominator) {
            Print();
        } else {
            int whole = Numerator / Denominator;
            int remainder = abs(Numerator) % Denominator;
            if (remainder == 0) {
                cout << whole;
            } else {
                cout << whole << ", " << remainder << "/" << Denominator;
            }
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

int main() {
    Fraction Result;
    char Operation;
    bool FirstInputs = true;

    cout << "Select Language(0 serbian, 1 english, 2 russian): ";
    cin >> LanguageIndex;

    while (true) {
        if (FirstInputs) {
            Fraction FirstFraction, SecondFraction;

            switch (LanguageIndex) {
                case 0: cout << "Unesite prvi razlomak"<<endl; break;
                case 1: cout << "Enter first fraction"<<endl; break;
                case 2: cout << "Введите первую дробь"<<endl; break;
            }
            FirstFraction.Input();

            switch (LanguageIndex) {
                case 0: cout << "Unesite drugi razlomak"<<endl; break;
                case 1: cout << "Enter second fraction"<<endl; break;
                case 2: cout << "Введите вторую дробь"<<endl; break;
            }
            SecondFraction.Input();

            switch (LanguageIndex) {
                case 0: cout << "Unesite operaciju (+, -, *, /): "; break;
                case 1: cout << "Enter operator (+, -, *, /): "; break;
                case 2: cout << "Выберите оператор (+, -, *, /): "; break;
            }
            cin >> Operation;
            switch (Operation) {
                case '+': Result = FirstFraction + SecondFraction; break;
                case '-': Result = FirstFraction - SecondFraction; break;
                case '*': Result = FirstFraction * SecondFraction; break;
                case '/': Result = FirstFraction / SecondFraction; break;
                default: return 0; break;
            }
            FirstInputs = false;

        } else {
            Fraction NextFraction;

            switch (LanguageIndex) {
                case 0: cout << "Unesite sledeci razlomak"; break;
                case 1: cout << "Enter next fraction"; break;
                case 2: cout << "Введи новую дробь"; break;
            }
            NextFraction.Input();
            switch (LanguageIndex) {
                case 0: cout << "Unesite operaciju (+, -, *, /): "; break;
                case 1: cout << "Enter operator (+, -, *, /): "; break;
                case 2: cout << "Выберите оператор (+, -, *, /): "; break;
            }
            cin >> Operation;

            switch (Operation) {
                case '+': Result = Result + NextFraction; break;
                case '-': Result = Result - NextFraction; break;
                case '*': Result = Result * NextFraction; break;
                case '/': Result = Result / NextFraction; break;
                default: return 0; break;
            }
        }

        switch (LanguageIndex) {
            case 0: cout << "Rezultat: "; break;
            case 1: cout << "Result: "; break;
            case 2: cout << "Результат: "; break;
        }
        Result.Print();
        switch (LanguageIndex) {
            case 0: cout << " (Mesoviti: "; break;
            case 1: cout << " (Mixed: "; break;
            case 2: cout << " (Смешаный: "; break;
        }
        Result.PrintMixed();
        cout << ")" << endl;

        char Choice;
        switch (LanguageIndex) {
            case 0: cout << "Da li zelite da nastavite sa jos operacija? (y/n): "; break;
            case 1: cout << "Do you want to continue with more operations? (y/n): "; break;
            case 2: cout << "Хотите ли вы продолжить с расчётными операциями? (y/n): ";  break;
        }
        cin >> Choice;
        if (Choice != 'y') break;
    }
}
