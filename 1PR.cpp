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

int main()
{
    int N, L, toFile;
    double x;

    cout << "N = ";  cin >> N;
    cout << "L = ";  cin >> L;
    cout << "X = ";  cin >> x;
    cout << "Write arrays to file? (1 - yes, 0 - no): ";  cin >> toFile;

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

    double start, tA, tB, tC, tD, tB1, tD1;

    // ===================== ИНИЦИАЛИЗАЦИЯ =====================

    // Массив A: a[0] = 0; a[i] = 10*x + a[i - 1]
    start = omp_get_wtime();
    a[0] = 0;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            a[i] = 10 * x + a[i - 1];
    tA = omp_get_wtime() - start;

    // Массив B: b[0] = x; b[i] = sin(x/i) + cos(i/n) + x / (n/10 - i)
    start = omp_get_wtime();
    b[0] = x;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++) {
            double t = N / 10.0 - i;   // знаменатель n/10 - i
            if (t == 0) t = 1;         // при i = N/10 знаменатель равен 0 - заменяем на 1, чтобы не делить на ноль
            b[i] = sin(x / i) + cos((double)i / N) + x / t;
        }
    tB = omp_get_wtime() - start;

    // Массив C: c[0] = 1; c[i] = b[i] * n - b[n - i]
    // При i = 1..N индекс n - i лежит в диапазоне 0..N-1, выхода за границы нет
    start = omp_get_wtime();
    c[0] = 1;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            c[i] = b[i] * N - b[N - i];
    tC = omp_get_wtime() - start;

    // Массив D: d[0] = x; d[i] = a[i - 1] + i - n
    start = omp_get_wtime();
    d[0] = x;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            d[i] = a[i - 1] + i - N;
    tD = omp_get_wtime() - start;

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
    tB1 = omp_get_wtime() - start;

    // Массив D: d[0] = x; d[i] = cos(a[i] / i) + c[i] / i
    start = omp_get_wtime();
    d[0] = x;
    for (int i = 1; i <= N; i++)
        for (int j = 0; j < L; j++)
            d[i] = cos(a[i] / i) + c[i] / i;
    tD1 = omp_get_wtime() - start;

    // Вывод значений ПОСЛЕ переопределения (вне замера времени)
    if (toFile == 1) {
        write_arrays(f, "AFTER redefinition:", a, b, c, d, N);
        f.close();
        cout << "Arrays are written to arrays.txt" << endl;
    }

    // Общее время вычислительной части = сумма времени всех циклов (без ввода/вывода)
    double total = tA + tB + tC + tD + tB1 + tD1;

    cout << fixed << setprecision(6);
    cout << endl << "Time, seconds:" << endl;
    cout << "A       = " << tA << endl;
    cout << "B       = " << tB << endl;
    cout << "C       = " << tC << endl;
    cout << "D       = " << tD << endl;
    cout << "B1      = " << tB1 << endl;
    cout << "D1      = " << tD1 << endl;
    cout << "Total   = " << total << endl;

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;

    return 0;
}