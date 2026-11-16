// Практическая работа 2. Маслаков Савелий, КИ24-07Б.
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

#include <climits>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>
#include <omp.h>
using namespace std;

void write_arrays(ofstream& file, const char* title, int N, int step,
                  const double* a, const double* b, const double* c, const double* d)
{
    if (!file.is_open())
        return;
    file << title << '\n' << setw(6) << "i" << setw(18) << "a[i]" << setw(18) << "b[i]"
         << setw(18) << "c[i]" << setw(18) << "d[i]" << '\n';
    for (int i = 0; ; i += min(step, N - i)) {
        file << setw(6) << i << setw(18) << a[i] << setw(18) << b[i]
             << setw(18) << c[i] << setw(18) << d[i] << '\n';
        if (i == N)
            break;
    }
    file << '\n';
}

// Этапы B, C, D, B1, D1 последовательной версии.
void sequential(int N, int L, double x, double* a, double* b, double* c, double* d,
                double t[], ofstream& file, int step)
{
    double start = omp_get_wtime();
    b[0] = x;
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) {
            double den = N / 10.0 - i;
            if (den == 0)
                den = 1;  // Как в ПР1.
            tmp = sin(x / i) + cos((double)i / N) + x / den;
        }
        b[i] = tmp;
    }
    t[1] = omp_get_wtime() - start;

    start = omp_get_wtime();
    c[0] = 1;
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = b[i] * N - b[N - i];
        c[i] = tmp;
    }
    t[2] = omp_get_wtime() - start;

    start = omp_get_wtime();
    d[0] = x;
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = a[i - 1] + i - N;
        d[i] = tmp;
    }
    t[3] = omp_get_wtime() - start;

    write_arrays(file, "BEFORE redefinition:", N, step, a, b, c, d);

    start = omp_get_wtime();
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = a[i] / i + c[i] / N;
        b[i] = tmp;
    }
    t[4] = omp_get_wtime() - start;

    start = omp_get_wtime();
    d[0] = x;
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = cos(a[i] / i) + c[i] / i;
        d[i] = tmp;
    }
    t[5] = omp_get_wtime() - start;
}

// Те же этапы с распределением итераций по потокам.
void parallel(int N, int L, double x, double* a, double* b, double* c, double* d,
                double t[], ofstream& file, int step)
{
    double start = omp_get_wtime();
    b[0] = x;
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) {
            double den = N / 10.0 - i;
            if (den == 0)
                den = 1;  // Как в ПР1.
            tmp = sin(x / i) + cos((double)i / N) + x / den;
        }
        b[i] = tmp;
    }
    t[1] = omp_get_wtime() - start;

    start = omp_get_wtime();
    c[0] = 1;
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = b[i] * N - b[N - i];
        c[i] = tmp;
    }
    t[2] = omp_get_wtime() - start;

    start = omp_get_wtime();
    d[0] = x;
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = a[i - 1] + i - N;
        d[i] = tmp;
    }
    t[3] = omp_get_wtime() - start;

    write_arrays(file, "BEFORE redefinition:", N, step, a, b, c, d);

    start = omp_get_wtime();
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = a[i] / i + c[i] / N;
        b[i] = tmp;
    }
    t[4] = omp_get_wtime() - start;

    start = omp_get_wtime();
    d[0] = x;
    #pragma omp parallel for schedule(runtime)
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = cos(a[i] / i) + c[i] / i;
        d[i] = tmp;
    }
    t[5] = omp_get_wtime() - start;
}

void compute(int N, int L, double x, bool use_parallel, double t[],
             ofstream& file, int step = 1)
{
    double* a = new double[N + 1];
    double* b = new double[N + 1];
    double* c = new double[N + 1];
    double* d = new double[N + 1];

    // A: текущий элемент зависит от предыдущего.
    double start = omp_get_wtime();
    a[0] = 0;
    for (int i = 1; i <= N; ++i) {
        double volatile tmp = 0;
        for (int j = 0; j < L; ++j) tmp = 10 * x + a[i - 1];
        a[i] = tmp;
    }
    t[0] = omp_get_wtime() - start;

    if (use_parallel)
        parallel(N, L, x, a, b, c, d, t, file, step);
    else
        sequential(N, L, x, a, b, c, d, t, file, step);

    write_arrays(file, "AFTER redefinition:", N, step, a, b, c, d);
    t[6] = t[0] + t[1] + t[2] + t[3] + t[4] + t[5];
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
}

void show_default_distribution(int N)
{
    vector<int> owner(N + 1);
    int threads = 0;
    #pragma omp parallel for
    for (int i = 1; i <= N; ++i) {
        owner[i] = omp_get_thread_num();
        if (i == 1)
            threads = omp_get_num_threads();
    }
    cout << "Default threads: " << threads << '\n';
    for (int first = 1; first <= N;) {
        int last = first;
        while (last < N && owner[last + 1] == owner[first])
            ++last;
        cout << "Thread " << owner[first] << ": i = " << first << ".." << last << '\n';
        first = last + 1;
    }
}

void print_row(int N, int L, const char* label, const double t[], double baseline = 0)
{
    cout << setw(8) << N << setw(6) << L << setw(6) << label;
    for (int i = 0; i < 7; ++i)
        cout << setw(11) << t[i];
    if (baseline > 0 && t[6] > 0)
        cout << setw(11) << baseline / t[6];
    cout << endl;
}

void report_point(int N, int L, double x, double seq[], double par[])
{
    ofstream no_file;
    compute(N, L, x, false, seq, no_file);
    compute(N, L, x, true, par, no_file);
    print_row(N, L, "Seq", seq);
    print_row(N, L, "Par", par, seq[6]);
}

int main()
{
    int mode;
    cout << "1 - sequential, 2 - parallel, 3 - report tables: ";
    if (!(cin >> mode) || mode < 1 || mode > 3)
        return 1;

    int N = 0, L = 100, save = 0, runs = 1;
    double x;
    if (mode == 3) {
        cout << "X: ";
        if (!(cin >> x) || !isfinite(x))
            return 1;
    } else {
        cout << "N (0 = array test), L, X: ";
        if (!(cin >> N >> L >> x) || N < 0 || N == INT_MAX || L < 1 || !isfinite(x))
            return 1;
        if (N == 0) {
            int cores = omp_get_num_procs();
            N = 100 * cores + cores / 2 - 1;
            cout << "Test: " << N + 1 << " elements, N = " << N << '\n';
            if (mode == 2)
                show_default_distribution(N);
        }
        cout << "Save arrays (0 no, 1 short, 2 full): ";
        if (!(cin >> save) || save < 0 || save > 2)
            return 1;
    }

    if (mode != 1) {
        int threads, schedule, chunk;
        cout << "Threads (0 default), schedule (1 static, 2 dynamic, 3 guided), chunk (0 default): ";
        if (!(cin >> threads >> schedule >> chunk) ||
            threads < 0 || schedule < 1 || schedule > 3 || chunk < 0)
            return 1;
        omp_sched_t policies[] = {omp_sched_static, omp_sched_dynamic, omp_sched_guided};
        omp_set_schedule(policies[schedule - 1], chunk);
        if (threads > 0) {
            omp_set_dynamic(0);
            omp_set_num_threads(threads);
        }
        if (mode == 2) {
            cout << "Parallel runs (1 or 5): ";
            if (!(cin >> runs) || (runs != 1 && runs != 5))
                return 1;
        }
        cout << "Requested threads = " << omp_get_max_threads() << ", schedule = "
             << schedule << ", chunk = " << chunk << ", X = " << x << '\n';
    }

    const char* header = "       N     L   Run          A          B          C          D"
                         "         B1         D1      Total    Speedup\n";
    cout << fixed << setprecision(6);
    if (mode == 3) {
        const int Lvalues[] = {1, 100, 200, 300, 400, 500};
        const int Nvalues[] = {100000, 200000, 300000, 400000, 500000};
        const int maxL = Lvalues[size(Lvalues) - 1];
        const int maxN = Nvalues[size(Nvalues) - 1];
        double seq_max[7], par_max[7], seq[7], par[7];

        cout << "\nTable 1. Vary L\n" << header;
        for (int l : Lvalues)
            report_point(maxN, l, x, seq_max, par_max);
        cout << "\nTable 2. Vary N\n" << header;
        for (int n : Nvalues)
            if (n < maxN)
                report_point(n, maxL, x, seq, par);
        print_row(maxN, maxL, "Seq", seq_max);
        print_row(maxN, maxL, "Par", par_max, seq_max[6]);
        return 0;
    }

    const char* filename = mode == 1 ? "arrays_seq.txt" : "arrays_par.txt";
    ofstream file;
    if (save != 0) {
        file.open(filename);
        if (!file) {
            cerr << "Could not open " << filename << '\n';
            return 1;
        }
        file << fixed << setprecision(6) << "N = " << N << ", L = " << L << ", X = " << x << "\n\n";
    }
    int step = 1;
    if (save == 1)
        while (N / step > 10)
            step *= 10;

    const char* label = mode == 1 ? "Seq" : "Par";
    cout << "Running " << label << ". N = " << N << ", L = " << L << ", X = " << x << endl;
    cout << header;
    double t[7], sum[7] = {};
    for (int run = 0; run < runs; ++run) {
        compute(N, L, x, mode == 2, t, file, step);
        print_row(N, L, label, t);
        for (int i = 0; i < 7; ++i)
            sum[i] += t[i];
        if (file.is_open())
            file.close();
    }
    if (!file) {
        cerr << "Could not write " << filename << '\n';
        return 1;
    }
    if (runs == 5) {
        for (double& value : sum)
            value /= runs;
        print_row(N, L, "Mean", sum);
    }
    if (save != 0)
        cout << "Saved " << filename << '\n';
}
