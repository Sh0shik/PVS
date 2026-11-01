// Практическая работа 1. Последовательная программа
// Маслаков Савелий, КИ24-07Б
// Вариант: A - 19, B - 1, C - 3, D - 28, B1 - 19, D1 - 28
//
// Инициализировать массивы A, B, C, D в соответствии с выражениями:
// a[0] = 0;  a[i] = 10*x + a[i - 1];
// b[0] = x;  b[i] = sin(x/i) + cos(i/n) + x / (n/10 - i);
// c[0] = 1;  c[i] = b[i] * n - b[n - i];
// d[0] = x;  d[i] = (a[i - 1] + i - n);
// Переопределить массивы B и D в соответствии с выражениями:
// b[0] = b[0];  b[i] = a[i] / i + c[i] / n;
// d[0] = x;     d[i] = cos(a[i] / i) + c[i] / i;
//
// i = 1..N, N - верхняя граница индексов, поэтому размер массивов N + 1 (индексы 0..N).
// L - трудоемкость одной итерации: каждое вычисление повторяется L раз (внутренний цикл по j).
//
// Режимы работы:
// 1 - один запуск: вводятся N, L, X, можно вывести массивы в файл (проверка правильности);
// 2 - эксперименты для отчета: таблица 1 (меняется L) и таблица 2 (меняется N).

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <omp.h>

using namespace std;

// Вывод всех четырех массивов в файл в виде таблицы
void write_arrays(ofstream& f, const char* title, double* a, double* b, double* c, double* d, int N)
{
    f << title << endl;
    f << setw(6) << "i" << setw(18) << "a[i]" << setw(18) << "b[i]"
      << setw(18) << "c[i]" << setw(18) << "d[i]" << endl;
    for (int i = 0; i <= N; i++)
        f << setw(6) << i << setw(18) << a[i] << setw(18) << b[i]
          << setw(18) << c[i] << setw(18) << d[i] << endl;
    f << endl;
}

// Вычисление всех массивов с замером времени каждого цикла.
// Результат: t[0]..t[5] - время A, B, C, D, B1, D1;  t[6] - общее время.
// Если toFile = 1, массивы до и после переопределения записываются в arrays.txt
void compute(int N, int L, double x, int toFile, double* t)
{
    // Выделение памяти - не входит в замер времени
    double* a = new double[N + 1];
    double* b = new double[N + 1];
    double* c = new double[N + 1];
    double* d = new double[N + 1];

    ofstream f;
    if (toFile == 1) {
        f.open("arrays.txt");
        f << fixed << setprecision(6);
        f << "N = " << N << ", L = " << L << ", X = " << x << endl << endl;
    }

    double start;

    // ===================== ИНИЦИАЛИЗАЦИЯ =====================

    // Массив A: a[0] = 0; a[i] = 10*x + a[i - 1]
    start = omp_get_wtime();
    a[0] = 0;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            a[i] = 10 * x + a[i - 1];
    t[0] = omp_get_wtime() - start;

    // Массив B: b[0] = x; b[i] = sin(x/i) + cos(i/n) + x / (n/10 - i)
    start = omp_get_wtime();
    b[0] = x;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++) {
            double den = N / 10.0 - i;   // знаменатель n/10 - i
            if (den == 0) den = 1;       // при i = N/10 знаменатель равен 0 - заменяем на 1, чтобы не делить на ноль
            b[i] = sin(x / i) + cos((double)i / N) + x / den;
        }
    t[1] = omp_get_wtime() - start;

    // Массив C: c[0] = 1; c[i] = b[i] * n - b[n - i]
    // При i = 1..N индекс n - i лежит в диапазоне 0..N-1, выхода за границы нет
    start = omp_get_wtime();
    c[0] = 1;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            c[i] = b[i] * N - b[N - i];
    t[2] = omp_get_wtime() - start;

    // Массив D: d[0] = x; d[i] = a[i - 1] + i - n
    start = omp_get_wtime();
    d[0] = x;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            d[i] = a[i - 1] + i - N;
    t[3] = omp_get_wtime() - start;

    // Вывод значений ДО переопределения (вне замера времени)
    if (toFile == 1)
        write_arrays(f, "BEFORE redefinition:", a, b, c, d, N);

    // ===================== ПЕРЕОПРЕДЕЛЕНИЕ =====================

    // Массив B: b[0] = b[0]; b[i] = a[i] / i + c[i] / n
    start = omp_get_wtime();
    // b[0] не меняется
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            b[i] = a[i] / i + c[i] / N;
    t[4] = omp_get_wtime() - start;

    // Массив D: d[0] = x; d[i] = cos(a[i] / i) + c[i] / i
    start = omp_get_wtime();
    d[0] = x;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            d[i] = cos(a[i] / i) + c[i] / i;
    t[5] = omp_get_wtime() - start;

    // Вывод значений ПОСЛЕ переопределения (вне замера времени)
    if (toFile == 1) {
        write_arrays(f, "AFTER redefinition:", a, b, c, d, N);
        f.close();
    }

    // Общее время вычислительной части = сумма времени всех циклов (без ввода/вывода)
    t[6] = t[0] + t[1] + t[2] + t[3] + t[4] + t[5];

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
}

// Заголовок таблицы замеров
void print_header()
{
    cout << "|      N |    L |" << setw(10) << "A" << " |" << setw(10) << "B" << " |"
         << setw(10) << "C" << " |" << setw(10) << "D" << " |" << setw(10) << "B1" << " |"
         << setw(10) << "D1" << " |" << setw(10) << "Total" << " |" << endl;
}

// Строка таблицы замеров: N, L и 7 значений времени
void print_row(int N, int L, double* t)
{
    cout << "|" << setw(7) << N << " |" << setw(5) << L << " |";
    for (int k = 0; k < 7; k++)
        cout << setw(10) << t[k] << " |";
    cout << endl;
}

int main()
{
    int mode, N, L, toFile;
    double x;
    double t[7];   // время A, B, C, D, B1, D1 и общее

    cout << fixed << setprecision(6);
    cout << "Mode (1 - single run, 2 - experiments for report): ";
    cin >> mode;

    if (mode == 1) {
        cout << "N = ";  cin >> N;
        cout << "L = ";  cin >> L;
        cout << "X = ";  cin >> x;
        cout << "Write arrays to file? (1 - yes, 0 - no): ";  cin >> toFile;

        compute(N, L, x, toFile, t);

        cout << endl << "Time, seconds:" << endl;
        print_header();
        print_row(N, L, t);
        if (toFile == 1)
            cout << "Arrays are written to arrays.txt" << endl;
    }

    if (mode == 2) {
        int Lvalues[6] = { 1, 100, 200, 300, 400, 500 };                 // значения L для таблицы 1
        int Nvalues[5] = { 100000, 200000, 300000, 400000, 500000 };     // значения N для таблицы 2

        cout << "X = ";  cin >> x;
        cout << "N for table 1 (L changes): ";  cin >> N;
        cout << "L for table 2 (N changes): ";  cin >> L;

        cout << endl << "Table 1. Time (s) depending on L, N = " << N << endl;
        print_header();
        for (int k = 0; k < 6; k++) {
            compute(N, Lvalues[k], x, 0, t);
            print_row(N, Lvalues[k], t);
        }

        cout << endl << "Table 2. Time (s) depending on N, L = " << L << endl;
        print_header();
        for (int k = 0; k < 5; k++) {
            compute(Nvalues[k], L, x, 0, t);
            print_row(Nvalues[k], L, t);
        }
    }

    return 0;
}